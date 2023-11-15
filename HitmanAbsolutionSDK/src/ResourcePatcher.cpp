#include "ResourcePatcher.h"
#include "ResourcePatcher.h"
#include <map>
#include <format>

#include <Glacier/Templates/TSharedPointer.h>
#include <Glacier/Resource/ZResourceLibraryInfo.h>
#include <Glacier/Resource/ZResourcePending.h>
#include <Glacier/Resource/ZResourceDataBuffer.h>
#include <Glacier/Resource/ZResourceLibraryLoader.h>
#include <Glacier/Resource/ZBufferBlock.h>
#include <Glacier/Resource/IResourceInstaller.h>

#include <ResourcePatcher.h>
#include <IO/BinaryReader.h>
#include <SDK.h>
#include <Hooks.h>

ResourcePatcher::ResourcePatcher()
{
    Hooks::ZHeaderLibraryInstaller_Install.CreateHook("ZHeaderLibraryInstaller::Install", 0x512940, ZHeaderLibraryInstaller_InstallHook);
    Hooks::ZResourceLibraryLoader_ProcessBlock.CreateHook("ZResourceLibraryLoader::ProcessBlock", 0x1F150, ZResourceLibraryLoader_ProcessBlockHook);
    Hooks::ZResourceLibraryLoader_AllocateEntry.CreateHook("ZResourceLibraryLoader::AllocateEntry", 0x156280, ZResourceLibraryLoader_AllocateEntryHook);
    Hooks::ZResourceLibraryLoader_StartLoading.CreateHook("ZResourceLibraryLoader::StartLoading", 0x1E87B0, ZResourceLibraryLoader_StartLoadingHook);

    Hooks::ZHeaderLibraryInstaller_Install.EnableHook();
    Hooks::ZResourceLibraryLoader_ProcessBlock.EnableHook();
    Hooks::ZResourceLibraryLoader_AllocateEntry.EnableHook();
    Hooks::ZResourceLibraryLoader_StartLoading.EnableHook();
}

void ResourcePatcher::LoadPatchedResources()
{
    if (patchedResources.contains(currentHeaderLibraryRuntimeResourceID.GetID()))
    {
        return;
    }

    const std::string currentHeaderLibraryResourceID = ZRuntimeResourceID::QueryResourceID(currentHeaderLibraryRuntimeResourceID).GetURI().ToCString();
    const size_t index2 = currentHeaderLibraryResourceID.find_last_of("/");
    const std::string headerLibraryFileName = currentHeaderLibraryResourceID.substr(index2 + 1, currentHeaderLibraryResourceID.find(".", index2) - index2 - 1);
    const std::string patchesFolderPath = std::format("ModManager\\patches\\{}", headerLibraryFileName);

    if (!std::filesystem::is_directory(patchesFolderPath))
    {
        return;
    }

    std::map<unsigned int, std::filesystem::path> patchFiles;

    for (const auto& entry : std::filesystem::directory_iterator(patchesFolderPath))
    {
        const std::string fileName = entry.path().filename().string();
        const unsigned int patchNumber = std::strtoul(fileName.substr(5, fileName.find(".") - 5).c_str(), nullptr, 10);

        patchFiles.insert(std::make_pair(patchNumber, entry.path()));
    }

    for (auto it = patchFiles.begin(); it != patchFiles.end(); ++it)
    {
        BinaryReader binaryReader = BinaryReader(it->second.string());
        const unsigned int patchedResourceCount = binaryReader.Read<unsigned int>();

        auto iterator = patchedResources.insert(std::make_pair(currentHeaderLibraryRuntimeResourceID.GetID(), std::vector<PatchedResource>())).first;

        for (unsigned int i = 0; i < patchedResourceCount; ++i)
        {
            PatchedResource patchedResource;

            patchedResource.runtimeResourceID = binaryReader.Read<unsigned long long>();
            patchedResource.resourceDataOffset = binaryReader.Read<unsigned int>();
            patchedResource.resourceDataSize = binaryReader.Read<unsigned int>();
            patchedResource.patchFileName = it->second.filename().string();

            iterator->second.push_back(patchedResource);
        }
    }
}

void ResourcePatcher::GetPatchedResource(const ZRuntimeResourceID& runtimeResourceID, void*& resourceData, unsigned int& resourceDataSize)
{
    const std::vector<PatchedResource>& patchedResources2 = patchedResources[currentHeaderLibraryRuntimeResourceID.GetID()];

    for (size_t i = 0; i < patchedResources2.size(); ++i)
    {
        if (patchedResources2[i].runtimeResourceID == runtimeResourceID.GetID())
        {
            const std::string currentHeaderLibraryResourceID = ZRuntimeResourceID::QueryResourceID(currentHeaderLibraryRuntimeResourceID).GetURI().ToCString();
            const size_t index2 = currentHeaderLibraryResourceID.find_last_of("/");
            const std::string headerLibraryFileName = currentHeaderLibraryResourceID.substr(index2 + 1, currentHeaderLibraryResourceID.find(".", index2) - index2 - 1);
            const std::string patchFilePath = std::format("ModManager\\patches\\{}\\{}", headerLibraryFileName, patchedResources2[i].patchFileName);
            BinaryReader binaryReader = BinaryReader(patchFilePath);

            binaryReader.Seek(patchedResources2[i].resourceDataOffset);

            resourceData = binaryReader.Read<void>(patchedResources2[i].resourceDataSize);
            resourceDataSize = patchedResources2[i].resourceDataSize;

            break;
        }
    }
}

void ResourcePatcher::SetCurrentHeaderLibraryRuntimeResourceID(const ZRuntimeResourceID& currentHeaderLibraryRuntimeResourceID)
{
    this->currentHeaderLibraryRuntimeResourceID = currentHeaderLibraryRuntimeResourceID;
}

const bool ResourcePatcher::IsResourcePatched(const ZRuntimeResourceID& runtimeResourceID) const
{
    auto patchedResourcesIterator = patchedResources.find(currentHeaderLibraryRuntimeResourceID.GetID());

    if (patchedResourcesIterator != patchedResources.end())
    {
        const std::vector<PatchedResource>& patchedResources2 = patchedResourcesIterator->second;

        for (size_t i = 0; i < patchedResources2.size(); ++i)
        {
            if (patchedResources2[i].runtimeResourceID == runtimeResourceID.GetID())
            {
                return true;
            }
        }
    }

    return false;
}

void ResourcePatcher::OnHeaderLibraryInstall(ZHeaderLibraryInstaller* headerLibraryInstaller, ZResourcePending& resourcePending)
{
    SDK::GetInstance().GetResourcePatcher()->SetCurrentHeaderLibraryRuntimeResourceID(resourcePending.GetResource().GetResourceStub()->GetRuntimeResourceID());
}

bool ResourcePatcher::OnProcessBlock(ZResourceLibraryLoader* resourceLibraryLoader, ZBufferBlock* block)
{
    unsigned int nReadSize = block->GetReadSize();
    unsigned char* pData = block->GetData();

    if (nReadSize < resourceLibraryLoader->GetEntryBytesLeft())
    {
        if (nReadSize)
        {
            resourceLibraryLoader->GetCurrentData()->Insert(pData, nReadSize);
            resourceLibraryLoader->SetEntryBytesLeft(resourceLibraryLoader->GetEntryBytesLeft() - nReadSize);
        }

        return true;
    }

    while (true)
    {
        unsigned int entryBytesLeft = resourceLibraryLoader->GetEntryBytesLeft();
        unsigned int entryIndex = resourceLibraryLoader->GetEntryIndex();
        ZResourceLibraryInfo* libraryEntries = resourceLibraryLoader->GetLibraryEntries();
        const SResourceLibraryEntry& entry = libraryEntries->GetEntry(entryIndex);

        if (IsResourcePatched(entry.pStub->GetRuntimeResourceID()))
        {
        }
        else
        {
            resourceLibraryLoader->GetCurrentData()->Insert(pData, entryBytesLeft);
        }

        pData += entryBytesLeft;
        nReadSize -= entryBytesLeft;

        if (libraryEntries->IsValid(entryIndex))
        {
            if (resourceLibraryLoader->GetCurrentData().GetTarget())
            {
                resourceLibraryLoader->GetCurrentData()->AddReference();
            }

            libraryEntries->InstallResource(entryIndex, resourceLibraryLoader->GetCurrentData());

            if (resourceLibraryLoader->GetCurrentData().GetTarget())
            {
                resourceLibraryLoader->GetCurrentData()->RemoveReference();
            }

            resourceLibraryLoader->GetCurrentData().SetTarget(nullptr);
        }

        if (entry.pStub->GetResourceStatus() != RESOURCE_STATUS_VALID)
        {
            if (resourceLibraryLoader->GetCurrentData().GetTarget())
            {
                resourceLibraryLoader->GetCurrentData()->RemoveReference();
            }

            resourceLibraryLoader->GetCurrentData().SetTarget(nullptr);
        }

        unsigned int nextEntryIndex = ++entryIndex;

        resourceLibraryLoader->SetEntryIndex(nextEntryIndex);

        if (nextEntryIndex >= libraryEntries->GetEntryCount())
        {
            return false;
        }

        const SResourceLibraryEntry& nextEntry = libraryEntries->GetEntry(nextEntryIndex);
        IResourceInstaller* resourceInstaller = nextEntry.pStub->GetResourceInstaller();
        const ZRuntimeResourceID& runtimeResourceID = nextEntry.pStub->GetRuntimeResourceID();

        resourceLibraryLoader->AllocateEntry(resourceInstaller, nextEntry.nDataSize, runtimeResourceID);

        if (nReadSize < resourceLibraryLoader->GetEntryBytesLeft())
        {
            if (nReadSize)
            {
                if (!IsResourcePatched(runtimeResourceID))
                {
                    resourceLibraryLoader->GetCurrentData()->Insert(pData, nReadSize);
                }

                resourceLibraryLoader->SetEntryBytesLeft(resourceLibraryLoader->GetEntryBytesLeft() - nReadSize);
            }

            return true;
        }
    }
}

void ResourcePatcher::OnAllocateEntry(ZResourceLibraryLoader* resourceLibraryLoader, IResourceInstaller* resourceInstaller, const unsigned int size, const ZRuntimeResourceID& runtimeResourceID)
{
    if (resourceInstaller->SupportsAllocate())
    {
        void* data = resourceInstaller->Allocate(size);
        TSharedPointer<ZResourceDataBuffer> resourceDataBuffer = ZResourceDataBuffer::Create(data, size);

        if (resourceDataBuffer.GetTarget() != resourceLibraryLoader->GetCurrentData().GetTarget())
        {
            if (resourceLibraryLoader->GetCurrentData().GetTarget())
            {
                resourceLibraryLoader->GetCurrentData()->RemoveReference();
            }

            resourceLibraryLoader->GetCurrentData().SetTarget(resourceDataBuffer.GetTarget());

            if (resourceLibraryLoader->GetCurrentData().GetTarget())
            {
                resourceLibraryLoader->GetCurrentData()->AddReference();
            }
        }
    }
    else
    {
        if (resourceLibraryLoader->GetCurrentData().GetTarget() != resourceLibraryLoader->GetFixedBuffer().GetTarget())
        {
            if (resourceLibraryLoader->GetCurrentData().GetTarget())
            {
                resourceLibraryLoader->GetCurrentData()->RemoveReference();
            }

            resourceLibraryLoader->GetCurrentData().SetTarget(resourceLibraryLoader->GetFixedBuffer().GetTarget());

            if (resourceLibraryLoader->GetCurrentData().GetTarget())
            {
                resourceLibraryLoader->GetCurrentData()->AddReference();
            }
        }

        resourceLibraryLoader->GetCurrentData()->Clear();
    }

    resourceLibraryLoader->SetEntryBytesLeft(size);
}

void ResourcePatcher::OnStartLoading(ZResourceLibraryLoader* resourceLibraryLoader)
{
    ZResourceLibraryInfo* libraryEntries = resourceLibraryLoader->GetLibraryEntries();
    unsigned int entryIndex = 0;
    unsigned int maxDataSize = 0;
    auto patchedResourcesIterator = patchedResources.find(currentHeaderLibraryRuntimeResourceID.GetID());

    if (patchedResourcesIterator != patchedResources.end())
    {
        const std::vector<PatchedResource>& patchedResources = patchedResourcesIterator->second;

        for (size_t i = 0; i < patchedResources.size(); ++i)
        {
            if (patchedResources[i].resourceDataSize > maxDataSize)
            {
                maxDataSize = patchedResources[i].resourceDataSize;
            }
        }
    }

    if (libraryEntries->GetEntryCount() > 0)
    {
        do
        {
            const SResourceLibraryEntry& entry = libraryEntries->GetEntry(entryIndex);

            if (entry.nDataSize > maxDataSize)
            {
                IResourceInstaller* resourceInstaller = entry.pStub->GetResourceInstaller();

                if (!resourceInstaller->SupportsAllocate())
                {
                    maxDataSize = entry.nDataSize;
                }
            }

            ++entryIndex;
        }
        while (entryIndex < libraryEntries->GetEntryCount());
    }

    IAllocator* normalAllocator = MemoryManager->GetNormalAllocator();
    void* data = normalAllocator->Allocate(maxDataSize, 0);
    TSharedPointer<ZResourceDataBuffer> resourceDataBuffer = ZResourceDataBuffer::Create(data, maxDataSize);

    if (resourceLibraryLoader->GetFixedBuffer().GetTarget() != resourceDataBuffer.GetTarget())
    {
        if (resourceLibraryLoader->GetFixedBuffer().GetTarget())
        {
            resourceLibraryLoader->GetFixedBuffer()->RemoveReference();
        }

        resourceLibraryLoader->GetFixedBuffer().SetTarget(resourceDataBuffer.GetTarget());

        if (resourceLibraryLoader->GetFixedBuffer().GetTarget())
        {
            resourceLibraryLoader->GetFixedBuffer()->AddReference();
        }
    }
}

bool __fastcall ZHeaderLibraryInstaller_InstallHook(ZHeaderLibraryInstaller* pThis, int edx, ZResourcePending& ResourcePending)
{
    SDK::GetInstance().GetResourcePatcher()->OnHeaderLibraryInstall(pThis, ResourcePending);

    return Hooks::ZHeaderLibraryInstaller_Install.CallOriginalFunction(pThis, ResourcePending);
}

bool __fastcall ZResourceLibraryLoader_ProcessBlockHook(ZResourceLibraryLoader* pThis, int edx, ZBufferBlock* pBlock)
{
    return SDK::GetInstance().GetResourcePatcher()->OnProcessBlock(pThis, pBlock);
}

void __fastcall ZResourceLibraryLoader_AllocateEntryHook(ZResourceLibraryLoader* pThis, int edx, IResourceInstaller* pInstaller, unsigned int nSize, ZRuntimeResourceID ridResource)
{
    SDK::GetInstance().GetResourcePatcher()->OnAllocateEntry(pThis, pInstaller, nSize, ridResource);
}

void __fastcall ZResourceLibraryLoader_StartLoadingHook(ZResourceLibraryLoader* pThis, int edx)
{
    SDK::GetInstance().GetResourcePatcher()->OnStartLoading(pThis);

    Hooks::ZResourceLibraryLoader_StartLoading.CallOriginalFunction(pThis);
}