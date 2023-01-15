#include "InternedString.hpp"
#include <city.h>
#include <iostream>

std::array<InternedString::SlotPool, InternedString::MAX_SLOT_POOL_ARRAY_SIZE> InternedString::_slotPoolArray = std::array<InternedString::SlotPool, InternedString::MAX_SLOT_POOL_ARRAY_SIZE>();
InternedString::StringEntryMemoryManager InternedString::_stringEntryMemoryManager = InternedString::StringEntryMemoryManager();

InternedString::InternedString()
	: _isNull1_StringEntryHandle29(0)
{
}

InternedString::InternedString(const std::string_view& string)
	: _isNull1_StringEntryHandle29(MakeInterned(string))
{
}

InternedString::InternedString(const InternedString& internedString)
	: _isNull1_StringEntryHandle29(internedString._isNull1_StringEntryHandle29)
{
}

InternedString::InternedString(InternedString&& internedString)
	: _isNull1_StringEntryHandle29(std::move(internedString._isNull1_StringEntryHandle29))
{
}

inline const uint32_t InternedString::MakeInterned(const std::string_view& string)
{
	if (string.size() >= 1024) std::cerr << "Interned string can only hold a string with a length less than 1024." << std::endl;

	const HashInfo&& hashInfo = GetHashInfo(string);

	InternedString::Slot* slot = _slotPoolArray[hashInfo.GetSlotPoolIndex()].FindTargetOrUnusedSlot(hashInfo);

	return 0;
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
	_stringEntryMemoryManager.currentMemoryBlockCursor = 0;
	_stringEntryMemoryManager.memoryBlockArray[0] = reinterpret_cast<char*>(std::malloc(MEMORY_BLOCK_INITIALIZE_SIZE));
	std::memset(_stringEntryMemoryManager.memoryBlockArray[0], 0, MEMORY_BLOCK_INITIALIZE_SIZE);
}

inline InternedString::Slot* InternedString::SlotPool::FindTargetOrUnusedSlot(const HashInfo& hashInfo) const
{
	const uint32_t CAPCITY_MASK = capcity - 1;
	for (uint32_t slotIndex = hashInfo.GetSlotIndex() & CAPCITY_MASK; true; slotIndex = (slotIndex + 1) & CAPCITY_MASK)
	{
		Slot& slot = slotArray[slotIndex];

		if (!slot.IsUsed() || slot.IsTargetSlot(hashInfo))
		{
			return &slot;
		}
	}
	return nullptr;
}

inline const bool InternedString::Slot::IsUsed() const
{
	return (slotHashProbe3_stringEntryHandle29 & IS_USED_MASK) == IS_USED_MASK;
}

inline const bool InternedString::Slot::IsTargetSlot(const HashInfo& hashInfo) const
{
	if (GetSlotHashProbe() == hashInfo.GetSlotHashProbe())
	{
		const StringEntry& stringEntry = _stringEntryMemoryManager.GetStringEntry(GetStringEntryHandle());
		if (stringEntry.GetStringEntryHeader() == hashInfo.GetStringEntryHeader() && 0 == strncmp(stringEntry.GetData(), hashInfo.GetData(), stringEntry.GetStringEntryHeader().GetSize()))
		{
			return true;
		}
	}
	return false;
}

inline const uint32_t InternedString::Slot::GetSlotHashProbe() const
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
	stringEntryHeader.SetStringHashProbe((Hi & STRING_HASH_PROBE_MASK) >> 8u);
	stringEntryHeader.SetSize(string.size());
	data = string.data();
}

inline const uint32_t InternedString::HashInfo::GetSlotHashProbe() const
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

inline InternedString::StringEntryHandle::StringEntryHandle(uint16_t memoryBlockIndex, uint16_t memoryBlockAlignedOffset)
	: empty3(0)
	, memoryBlockIndex13(memoryBlockIndex)
	, memoryBlockAlignedOffset16(memoryBlockAlignedOffset)
{
}

inline const uint16_t InternedString::StringEntryHandle::GetMemoryBlockIndex() const
{
	return memoryBlockIndex13;
}

inline const uint16_t InternedString::StringEntryHandle::GetMemoryBlockAlignedOffset() const
{
	return memoryBlockAlignedOffset16;
}

inline const InternedString::StringEntry& InternedString::StringEntryMemoryManager::GetStringEntry(const StringEntryHandle& stringEntryHandle) const
{
	return *reinterpret_cast<InternedString::StringEntry*>(memoryBlockArray[stringEntryHandle.GetMemoryBlockIndex()] + stringEntryHandle.GetMemoryBlockAlignedOffset() * 2);
}

inline const InternedString::StringEntryHeader InternedString::StringEntry::GetStringEntryHeader() const
{
	return stringEntryHeader;
}

inline const char* InternedString::StringEntry::GetData() const
{
	return data;
}

inline const bool InternedString::StringEntryHeader::operator==(const StringEntryHeader& r) const
{
	return stringHashProbe6 == r.stringHashProbe6 && size10 && r.size10;
}

inline void InternedString::StringEntryHeader::SetStringHashProbe(uint8_t stringHashProbe)
{
	stringHashProbe6 = stringHashProbe;
}

inline void InternedString::StringEntryHeader::SetSize(uint16_t size)
{
	size10 = size;
}

inline const uint16_t InternedString::StringEntryHeader::GetSize() const
{
	return size10;
}
