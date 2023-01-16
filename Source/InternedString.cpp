#include "InternedString.hpp"
#include <city.h>
#include <iostream>

#define Align(size, align) (((size) + (align) - 1) & ~((align) - 1));

std::array<InternedString::SlotPool, InternedString::MAX_SLOT_POOL_ARRAY_SIZE> InternedString::_slotPoolArray = std::array<InternedString::SlotPool, InternedString::MAX_SLOT_POOL_ARRAY_SIZE>();
InternedString::StringEntryMemoryManager InternedString::_stringEntryMemoryManager = InternedString::StringEntryMemoryManager();

InternedString::InternedString()
	: _empty2_isUsed1_StringEntryHandle29(0)
{
}

InternedString::InternedString(const std::string_view& string)
	: _empty2_isUsed1_StringEntryHandle29(MakeInterned(string))
{
}

InternedString::InternedString(const InternedString& internedString)
	: _empty2_isUsed1_StringEntryHandle29(internedString._empty2_isUsed1_StringEntryHandle29)
{
}

void InternedString::operator=(const InternedString& internedString)
{
	_empty2_isUsed1_StringEntryHandle29 = internedString._empty2_isUsed1_StringEntryHandle29;
}

InternedString::InternedString(InternedString&& internedString)
	: _empty2_isUsed1_StringEntryHandle29(std::move(internedString._empty2_isUsed1_StringEntryHandle29))
{
}

void InternedString::operator=(InternedString&& internedString)
{
	_empty2_isUsed1_StringEntryHandle29 = std::move(internedString._empty2_isUsed1_StringEntryHandle29);
}

std::string_view InternedString::ToStringView() const
{
	const StringEntry* stringEntry = _stringEntryMemoryManager.GetStringEntry(StringEntryHandle(_empty2_isUsed1_StringEntryHandle29));
	return std::string_view(stringEntry->GetData(), stringEntry->GetStringEntryHeader().GetSize());
}

std::string InternedString::ToString() const
{
	const StringEntry* stringEntry = _stringEntryMemoryManager.GetStringEntry(StringEntryHandle(_empty2_isUsed1_StringEntryHandle29));
	return std::string(stringEntry->GetData(), stringEntry->GetStringEntryHeader().GetSize());
}

inline const uint32_t InternedString::MakeInterned(const std::string_view& string)
{
	if (string.size() > MAX_STRING_SIZE) std::cerr << "Interned string can only hold a string with a length less than 1024." << std::endl;

	const HashInfo&& hashInfo = GetHashInfo(string);

	InternedString::Slot* slot = _slotPoolArray[hashInfo.GetSlotPoolIndex()].FindTargetOrUnusedSlot(hashInfo);

	if (!slot->IsUsed())
	{
		_slotPoolArray[hashInfo.GetSlotPoolIndex()].size++;
		const StringEntryHandle&& stringEntryHandle = _stringEntryMemoryManager.AllocateStringEntry(hashInfo.GetStringEntryHeader(), hashInfo.GetData());
		slot->Load(hashInfo.GetSlotHashProbeValue(), stringEntryHandle);
	}

	return slot->GetSlotValue() & (IS_USED_MASK | STRING_ENTRY_HANDLE_MASK);
}

inline const InternedString::HashInfo InternedString::GetHashInfo(const std::string_view& string)
{
	uint64_t hash = CityHash64(string.data(), string.size());

	return HashInfo(hash, string);
}

void InternedString::Initialize()
{
	for (auto& slotPool : _slotPoolArray)
	{
		slotPool.size = 0;
		slotPool.capcity = SLOT_POOL_INITIALIZE_SIZE;
		slotPool.slotArray = reinterpret_cast<Slot*>(std::malloc(SLOT_POOL_INITIALIZE_SIZE * sizeof(Slot)));
		std::memset(slotPool.slotArray, 0, SLOT_POOL_INITIALIZE_SIZE);
	}

	_stringEntryMemoryManager.currentMemoryBlockIndex = 0;
	_stringEntryMemoryManager.currentMemoryBlockAlignedCursor = 0;
	_stringEntryMemoryManager.memoryBlockArray[0] = reinterpret_cast<char*>(std::malloc(MAX_MEMORY_BLOCK_SIZE));
	std::memset(_stringEntryMemoryManager.memoryBlockArray[0], 0, MAX_MEMORY_BLOCK_SIZE);
}

inline InternedString::Slot* InternedString::SlotPool::FindTargetOrUnusedSlot(const HashInfo& hashInfo)
{
	if (size > SLOT_POOL_RESIZE_USAGE_RATE * capcity)
	{
		Resize();
	}

	const uint32_t SLOT_POOL_CAPCITY_MASK = capcity - 1;
	for (uint32_t slotIndex = hashInfo.GetSlotIndex() & SLOT_POOL_CAPCITY_MASK; true; slotIndex = (slotIndex + 1) & SLOT_POOL_CAPCITY_MASK)
	{
		Slot* slot = &(slotArray[slotIndex]);

		if (!slot->IsUsed() || slot->IsTargetSlot(hashInfo))
		{
			return slot;
		}
	}
	return nullptr;
}

inline InternedString::Slot* InternedString::SlotPool::FindUnusedSlot(const HashInfo& hashInfo)
{
	const uint32_t SLOT_POOL_CAPCITY_MASK = capcity - 1;
	for (uint32_t slotIndex = hashInfo.GetSlotIndex() & SLOT_POOL_CAPCITY_MASK; true; slotIndex = (slotIndex + 1) & SLOT_POOL_CAPCITY_MASK)
	{
		Slot* slot = &(slotArray[slotIndex]);

		if (!slot->IsUsed())
		{
			return slot;
		}
	}
	return nullptr;
}

void InternedString::SlotPool::Resize()
{
	uint32_t oldCapcity = capcity;
	uint32_t oldSize = size;
	Slot* oldSlotArray = slotArray;

	capcity = oldSize * 2;
	size = oldSize;
	slotArray = reinterpret_cast<Slot*>(std::malloc(capcity * sizeof(Slot)));
	std::memset(slotArray, 0, SLOT_POOL_INITIALIZE_SIZE);

	for (uint32_t slotIndex = 0; slotIndex < oldSize; slotIndex++)
	{
		StringEntry* stringEntry = _stringEntryMemoryManager.GetStringEntry(oldSlotArray[slotIndex].GetStringEntryHandle());
		std::string_view string(stringEntry->GetData(), stringEntry->GetStringEntryHeader().GetSize());

		const HashInfo&& hashInfo = GetHashInfo(string);

		InternedString::Slot* slot = FindUnusedSlot(hashInfo);

		const StringEntryHandle&& stringEntryHandle = _stringEntryMemoryManager.AllocateStringEntry(hashInfo.GetStringEntryHeader(), hashInfo.GetData());
		slot->Load(hashInfo.GetSlotHashProbeValue(), stringEntryHandle);
	}

	std::free(oldSlotArray);
}

inline const bool InternedString::Slot::IsUsed() const
{
	return (slotHashProbe3_stringEntryHandle29 & IS_USED_MASK) == IS_USED_MASK;
}

inline const bool InternedString::Slot::IsTargetSlot(const HashInfo& hashInfo) const
{
	if (GetSlotHashProbeValue() == hashInfo.GetSlotHashProbeValue())
	{
		const StringEntry* stringEntry = _stringEntryMemoryManager.GetStringEntry(GetStringEntryHandle());
		if (stringEntry->GetStringEntryHeader() == hashInfo.GetStringEntryHeader() && 0 == strncmp(stringEntry->GetData(), hashInfo.GetData(), stringEntry->GetStringEntryHeader().GetSize()))
		{
			return true;
		}
	}
	return false;
}

inline const uint32_t InternedString::Slot::GetSlotHashProbeValue() const
{
	constexpr uint32_t SLOT_HASH_PROBE_BITS = 3u;
	constexpr uint32_t SLOT_HASH_PROBE_MASK = ((1u << SLOT_HASH_PROBE_BITS) - 1) << 29u;

	return slotHashProbe3_stringEntryHandle29 & SLOT_HASH_PROBE_MASK;
}

inline const InternedString::StringEntryHandle InternedString::Slot::GetStringEntryHandle() const
{
	constexpr uint32_t OFFSET_BITS = 16u;
	constexpr uint32_t OFFSET_MASK = (1u << OFFSET_BITS) - 1u;
	constexpr uint32_t INDEX_BITS = 13u;
	constexpr uint32_t INDEX_MASK = (1u << INDEX_BITS) - 1u;

	return StringEntryHandle((slotHashProbe3_stringEntryHandle29 & INDEX_MASK) >> 16u , slotHashProbe3_stringEntryHandle29 & OFFSET_MASK);
}

inline const uint32_t InternedString::Slot::GetStringEntryHandleValue() const
{
	constexpr uint32_t STRING_ENTRY_HANDLE_BITS = 29u;
	constexpr uint32_t STRING_ENTRY_HANDLE_MASK = (1u << STRING_ENTRY_HANDLE_BITS) - 1u;

	return slotHashProbe3_stringEntryHandle29 & STRING_ENTRY_HANDLE_BITS;
}

inline const uint32_t InternedString::Slot::GetSlotValue() const
{
	return slotHashProbe3_stringEntryHandle29;
}

inline void InternedString::Slot::Load(const uint32_t slotHashProbeValue, const StringEntryHandle& stringEntryHandle)
{
	slotHashProbe3_stringEntryHandle29 = slotHashProbeValue | stringEntryHandle.GetStringEntryHandleValue() | IS_USED_MASK;
}

inline InternedString::HashInfo::HashInfo(uint64_t hash, const std::string_view& string)
{
	uint32_t Hi = static_cast<uint32>(hash >> 32);
	uint32_t Lo = static_cast<uint32>(hash);

	constexpr uint32_t SLOT_POOL_INDEX_BITS = 8u;
	constexpr uint32_t SLOT_POOL_INDEX_MASK = (1u << SLOT_POOL_INDEX_BITS) - 1u;
	constexpr uint32_t SLOT_HASH_PROBE_BITS = 3u;
	constexpr uint32_t SLOT_HASH_PROBE_MASK = ((1u << SLOT_HASH_PROBE_BITS) - 1) << 29u;
	constexpr uint32_t STRING_HASH_PROBE_BITS = 6u;
	constexpr uint32_t STRING_HASH_PROBE_MASK = ((1u << STRING_HASH_PROBE_BITS) - 1) << 8u;

	slotIndex32 = Lo;
	slotHashProbe3_empty29 = Hi & SLOT_HASH_PROBE_MASK;
	slotPoolIndex8 = Hi & SLOT_POOL_INDEX_MASK;
	stringEntryHeader.SetStringHashProbeAndSize((Hi & STRING_HASH_PROBE_MASK) >> 8u, string.size());
	data = string.data();
}

inline const uint32_t InternedString::HashInfo::GetSlotHashProbeValue() const
{
	return slotHashProbe3_empty29;
}

inline const InternedString::StringEntryHeader InternedString::HashInfo::GetStringEntryHeader() const
{
	return stringEntryHeader;
}

inline const char* InternedString::HashInfo::GetData() const
{
	return data;
}

inline const uint32_t InternedString::HashInfo::GetSlotIndex() const
{
	return slotIndex32;
}

inline const uint8_t InternedString::HashInfo::GetSlotPoolIndex() const
{
	return slotPoolIndex8;
}

inline InternedString::StringEntryHandle::StringEntryHandle(uint32_t stringEntryHandleValue)
	: StringEntryHandle(static_cast<uint16_t>(stringEntryHandleValue >> 16) & MEMORY_BLOCK_INDEX_MASK, static_cast<uint16_t>(stringEntryHandleValue))
{
}

inline InternedString::StringEntryHandle::StringEntryHandle(uint16_t memoryBlockIndex, uint16_t memoryBlockAlignedOffset)
	: empty3_memoryBlockIndex13(MEMORY_BLOCK_INDEX_BITS & memoryBlockIndex)
	, memoryBlockAlignedOffset16(memoryBlockAlignedOffset)
{
}

inline const uint16_t InternedString::StringEntryHandle::GetMemoryBlockIndex() const
{
	return empty3_memoryBlockIndex13;
}

inline const uint16_t InternedString::StringEntryHandle::GetMemoryBlockAlignedOffset() const
{
	return memoryBlockAlignedOffset16;
}

inline const uint32_t InternedString::StringEntryHandle::GetStringEntryHandleValue() const
{
	return static_cast<uint32_t>(empty3_memoryBlockIndex13) << 16 | memoryBlockAlignedOffset16;
}

inline InternedString::StringEntry* InternedString::StringEntryMemoryManager::GetStringEntry(const StringEntryHandle& stringEntryHandle) const
{
	return reinterpret_cast<InternedString::StringEntry*>(memoryBlockArray[stringEntryHandle.GetMemoryBlockIndex()] + stringEntryHandle.GetMemoryBlockAlignedOffset() * 2);
}

inline const InternedString::StringEntryHandle InternedString::StringEntryMemoryManager::AllocateStringEntry(const StringEntryHeader& stringEntryHeader, const char* data)
{
	const uint32_t size = Align(sizeof(StringEntryHeader) + stringEntryHeader.GetSize(), 2);
	const uint32_t alignedSize = size >> 1;
	if (((static_cast<uint32_t>(currentMemoryBlockAlignedCursor) + alignedSize) << 1) >= MAX_MEMORY_BLOCK_SIZE)
	{
		CreateNewMemoryBlock();
	}

	StringEntryHandle stringEntryHandle(currentMemoryBlockIndex, currentMemoryBlockAlignedCursor);
	currentMemoryBlockAlignedCursor += alignedSize;

	StringEntry* stringEntry = GetStringEntry(stringEntryHandle);
	stringEntry->SetStringEntryHeader(stringEntryHeader);
	memcpy(const_cast<char*>(stringEntry->GetData()), data, stringEntryHeader.GetSize());

	return stringEntryHandle;
}

inline void InternedString::StringEntryMemoryManager::CreateNewMemoryBlock()
{
	if (_stringEntryMemoryManager.currentMemoryBlockIndex >= (MAX_MEMORY_BLOCK_ARRAY_SIZE - 1)) std::cerr << "Interned string only supports a maximum of 1GB memory." << std::endl;
	
	_stringEntryMemoryManager.currentMemoryBlockIndex += 1;
	_stringEntryMemoryManager.currentMemoryBlockAlignedCursor = 0;
	_stringEntryMemoryManager.memoryBlockArray[_stringEntryMemoryManager.currentMemoryBlockIndex] = reinterpret_cast<char*>(std::malloc(MAX_MEMORY_BLOCK_SIZE));
	std::memset(_stringEntryMemoryManager.memoryBlockArray[_stringEntryMemoryManager.currentMemoryBlockIndex], 0, MAX_MEMORY_BLOCK_SIZE);
}

inline const InternedString::StringEntryHeader InternedString::StringEntry::GetStringEntryHeader() const
{
	return stringEntryHeader;
}

inline void InternedString::StringEntry::SetStringEntryHeader(const StringEntryHeader& stringEntryHeader)
{
	this->stringEntryHeader = stringEntryHeader;
}

inline const char* InternedString::StringEntry::GetData() const
{
	return data;
}

inline const bool InternedString::StringEntryHeader::operator==(const StringEntryHeader& r) const
{
	return stringHashProbe6_size10 == r.stringHashProbe6_size10;
}

inline void InternedString::StringEntryHeader::SetStringHashProbeAndSize(const uint8_t stringHashProbe, const uint16_t size)
{
	stringHashProbe6_size10 = (STRING_HASH_PROBE_MASK & (static_cast<uint16_t>(stringHashProbe) << 10u)) | (SIZE_MASK & size);
}

inline const uint16_t InternedString::StringEntryHeader::GetSize() const
{
	return SIZE_MASK & stringHashProbe6_size10;
}
