#include "qemu/osdep.h"
#include "hw/pci/msi.h"
#include "hw/pci/pci.h"

#define TYPE_MY_RNG "my_rng"
#define MY_RNG(obj) OBJECT_CHECK(my_rng, (obj), TYPE_MY_RNG)
#define REGISTER_SIZE 4 //in bytes
#define RNG_REGISTER 0x0
#define SEED_REGISTER 0x4

typedef struct {
    PCIDevice parent_obj;
    uint32_t seed_register;
    MemoryRegion mmio;
} my_rng;

/**
 * Gives a pseudo-random number depending on the SEED found in the my_rng structure
 * submitted. If the offset corresponds to the seed register, returns the seed value
 * @opaque: device, initially of type (my_rng)
 * @addr: contains the offset from the base address in the area of memory mapped
 * I/O at which the read operation takes place
 * @size: size of the read operation
 */
static uint64_t mmio_read(void* opaque, hwaddr addr, unsigned size){
    uint64_t value=0;
    if (size > REGISTER_SIZE || opaque == NULL){ //do not allow buffer overflow
        printf("wrong parameters - read\n");
        return -1;
    }
    if (addr == RNG_REGISTER){
        value = (uint64_t)rand();

    } else if (addr == SEED_REGISTER){
        my_rng* device = (my_rng*)opaque;
        value = device->seed_register;
    }
    return value;
}


/**
 * Writes the seed value in the virtual device's seed register and initializes the
 * seed. Does not do any of this if the addr is wrong or the size too big
 * @opaque: device, initially of type (my_rng)
 * @addr: contains the offset from the base address in the area of memory mapped
 * I/O at which the write operation takes place
 * @size: size of the write operation
 */
static void mmio_write(void* opaque, hwaddr addr, uint64_t val, unsigned size){
    if (size > REGISTER_SIZE || addr != SEED_REGISTER || opaque == NULL ){ //do not allow buffer overflow
        printf("Wrong parameters - write\n");
        return;
    }
    my_rng* device = (my_rng*)opaque;
    device->seed_register = (uint32_t)val;
    srand(device->seed_register);
    return;
}

static const MemoryRegionOps my_rng_ops = {
    .read = mmio_read,
    .write = mmio_write,
};

/**
 * Function that initialises an instance of the virtual random number generator by
 * creating a region of I/O memory for the memory mapped registers with memory_region_init
 * memory_region_init has a size of 4 KB.
 * The function also registers the device on the PCI bus with pci_register_bar
 */
static void my_rng_realize(PCIDevice *pdev, Error **errp) {
    my_rng *s = MY_RNG(pdev);
    memory_region_init_io(&s->mmio, OBJECT(s), &my_rng_ops, s,
                          "my_rng", 4096);
    pci_register_bar(&s->parent_obj, 0, PCI_BASE_ADDRESS_SPACE_MEMORY, &s->mmio);
}

/**
 * Runs once when Qemu starts. Defines a few characteristics common to all
 * instances of our virtual device
 */
static void my_rng_class_init(ObjectClass *class, void *data) {
    DeviceClass *dc = DEVICE_CLASS(class);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(class);

    k->realize = my_rng_realize;
    k->vendor_id = PCI_VENDOR_ID_QEMU;
    k->device_id = 0xcafe;
    k->revision = 0x10;
    k->class_id = PCI_CLASS_OTHERS;
    
    set_bit(DEVICE_CATEGORY_MISC, dc->categories);
}

static void my_rng_register_types(void) {
    static InterfaceInfo interfaces[] = {
        { INTERFACE_CONVENTIONAL_PCI_DEVICE },
        { },
    };

    static const TypeInfo my_rng_info = {
        .name = TYPE_MY_RNG,
        .parent = TYPE_PCI_DEVICE,
        .instance_size = sizeof(my_rng),
        .class_init    = my_rng_class_init,
        .interfaces = interfaces,
    };

    type_register_static(&my_rng_info);
}

type_init(my_rng_register_types)
