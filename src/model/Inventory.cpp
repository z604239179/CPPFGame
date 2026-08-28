#include "model/Inventory.h"

#include <cstdlib>
#include <utility>

#include "model/Consumable.h"
#include "model/Equipment.h"
#include "model/GameData.h"

namespace game {

Inventory::Inventory() : slots_(kInventorySlots) {}

bool Inventory::addItem(std::unique_ptr<Item> item) {
    if (!item) return false;

    // 消耗品尝试堆叠
    if (item->typeTag() == 'C') {
        for (auto& slot : slots_) {
            if (slot && slot->typeTag() == 'C' && slot->id == item->id) {
                slot->count += item->count;
                return true;
            }
        }
    }

    const int empty = firstEmptySlot();
    if (empty < 0) return false;
    slots_[empty] = std::move(item);
    return true;
}

bool Inventory::useConsumable(int slot, int& restoreHp, int& restoreMp) {
    if (slot < 0 || slot >= kInventorySlots) return false;
    auto& item = slots_[slot];
    if (!item || item->typeTag() != 'C') return false;

    const auto* cons = static_cast<Consumable*>(item.get());
    restoreHp = cons->restoreHp;
    restoreMp = cons->restoreMp;
    if (--item->count <= 0) slots_[slot].reset();
    return true;
}

bool Inventory::equip(int slot) {
    if (slot < 0 || slot >= kInventorySlots) return false;
    auto& item = slots_[slot];
    if (!item || item->typeTag() != 'E') return false;

    const auto* eq = static_cast<Equipment*>(item.get());
    const int idx = static_cast<int>(eq->slot);
    std::swap(equipment_[idx], slots_[slot]);
    return true;
}

bool Inventory::sell(int slot, int& goldGain) {
    if (slot < 0 || slot >= kInventorySlots) return false;
    auto& item = slots_[slot];
    if (!item) return false;
    goldGain = item->price;
    slots_[slot].reset();
    return true;
}

bool Inventory::drop(int slot) {
    if (slot < 0 || slot >= kInventorySlots || !slots_[slot]) return false;
    slots_[slot].reset();
    return true;
}

Item* Inventory::itemAt(int slot) {
    return (slot >= 0 && slot < kInventorySlots) ? slots_[slot].get() : nullptr;
}

const Item* Inventory::itemAt(int slot) const {
    return (slot >= 0 && slot < kInventorySlots) ? slots_[slot].get() : nullptr;
}

Item* Inventory::equippedAt(EquipmentSlot slot) {
    const int idx = static_cast<int>(slot);
    return (idx >= 0 && idx < kEquipmentSlots) ? equipment_[idx].get() : nullptr;
}

const Item* Inventory::equippedAt(EquipmentSlot slot) const {
    const int idx = static_cast<int>(slot);
    return (idx >= 0 && idx < kEquipmentSlots) ? equipment_[idx].get() : nullptr;
}

int Inventory::itemCount() const {
    int n = 0;
    for (const auto& slot : slots_) {
        if (slot) ++n;
    }
    return n;
}

int Inventory::firstEmptySlot() const {
    for (int i = 0; i < kInventorySlots; ++i) {
        if (!slots_[i]) return i;
    }
    return -1;
}

bool Inventory::isFull() const { return firstEmptySlot() < 0; }

std::string Inventory::serializeItems() const {
    std::string out;
    for (int i = 0; i < kInventorySlots; ++i) {
        if (!slots_[i]) continue;
        if (!out.empty()) out += ",";
        out += std::to_string(i) + ":" + slots_[i]->typeTag() + ":" +
               std::to_string(slots_[i]->id) + ":" + std::to_string(slots_[i]->count);
    }
    return out;
}

std::string Inventory::serializeEquipment() const {
    std::string out;
    for (int i = 0; i < kEquipmentSlots; ++i) {
        if (!equipment_[i]) continue;
        if (!out.empty()) out += ",";
        out += std::to_string(i) + ":" + equipment_[i]->typeTag() + ":" +
               std::to_string(equipment_[i]->id);
    }
    return out;
}

bool Inventory::loadItems(const std::string& data) {
    for (const auto& seg : splitString(data, ',')) {
        if (seg.empty()) continue;
        const auto f = splitString(seg, ':');
        if (f.size() != 4) continue;
        const int slot = std::atoi(f[0].c_str());
        if (slot < 0 || slot >= kInventorySlots || slots_[slot]) continue;
        const char tag = f[1].empty() ? '0' : f[1][0];
        auto item = GameData::createItem(tag, std::atoi(f[2].c_str()), std::atoi(f[3].c_str()));
        if (item) slots_[slot] = std::move(item);
    }
    return true;
}

bool Inventory::loadEquipment(const std::string& data) {
    for (const auto& seg : splitString(data, ',')) {
        if (seg.empty()) continue;
        const auto f = splitString(seg, ':');
        if (f.size() != 3 || f[0].empty() || f[1].empty() || f[0][0] == '0') continue;
        const int slot = std::atoi(f[0].c_str());
        if (slot < 0 || slot >= kEquipmentSlots) continue;
        auto item = GameData::createItem(f[1][0], std::atoi(f[2].c_str()), 1);
        if (item && item->typeTag() == 'E') {
            equipment_[slot] = std::move(item);
        }
    }
    return true;
}

}  // namespace game
