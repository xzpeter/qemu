/*
 * Internal definitions for a target's KVM support
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 *
 */

#ifndef QEMU_KVM_INT_H
#define QEMU_KVM_INT_H

#include "exec/memory.h"
#include "sysemu/accel.h"
#include "sysemu/kvm.h"

typedef struct KVMSlot
{
    hwaddr start_addr;
    ram_addr_t memory_size;
    void *ram;
    int slot;
    int flags;
    int old_flags;
    /* Dirty bitmap cache for the slot */
    unsigned long *dirty_bmap;
} KVMSlot;

typedef struct KVMMemoryListener {
    MemoryListener listener;
    KVMSlot *slots;
    int as_id;
} KVMMemoryListener;

#define TYPE_KVM_ACCEL ACCEL_CLASS_NAME("kvm")

#define KVM_STATE(obj) \
    OBJECT_CHECK(KVMState, (obj), TYPE_KVM_ACCEL)

void kvm_memory_listener_register(KVMState *s, KVMMemoryListener *kml,
                                  AddressSpace *as, int as_id);

void kvm_set_max_memslot_size(hwaddr max_slot_size);

/**
 * kvm_hwpoison_page_add:
 *
 * Parameters:
 *  @ram_addr: the address in the RAM for the poisoned page
 *
 * Add a poisoned page to the list
 *
 * Return: None.
 */
void kvm_hwpoison_page_add(ram_addr_t ram_addr);
#endif
