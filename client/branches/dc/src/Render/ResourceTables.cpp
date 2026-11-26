// Sin PCH: gestión de páginas Win32/POSIX sin dependencias del renderer.
#include "Render/ResourceTables.h"
#include "Core/ResourceLimits.h"
#include "Model/ModelTypes.h"
#include <cstring>
#include <limits>
#include <new>
#include <stdexcept>
#ifdef _WIN32
#include <windows.h>
#elif defined(__DREAMCAST__)
// KOS no tiene sys/mman: el pool se reserva de una vez con calloc (ver ctor).
#include <cstdlib>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif
namespace ResourceTables {
namespace {
class Table {
    char* data=nullptr;
    static constexpr std::size_t addressable=std::size_t((std::numeric_limits<short>::max)())+1;
    std::size_t limit_=0,committed=0,reserved=0,page=0;
    std::size_t stride,count=0;
    std::size_t Align(std::size_t value) const { return (value+page-1)/page*page; }
public:
    explicit Table(std::size_t slotStride, std::size_t reserve):stride(slotStride) {
#ifdef _WIN32
        SYSTEM_INFO info;GetSystemInfo(&info);page=info.dwPageSize;
        limit_=addressable;   // PC: reserva el espacio de direcciones completo (lazy commit)
        reserved=Align(limit_*stride);
        data=(char*)VirtualAlloc(nullptr,reserved,MEM_RESERVE,PAGE_NOACCESS);
#elif defined(__DREAMCAST__)
        page=4096;  // nominal; no se reclama nada en runtime
        // DC: no hay commit perezoso posible con calloc; reservar el cupo
        // vanilla real (MODEL/BITMAP_MAX_VANILLA) en vez de los 32768 slots
        // representables, que son ~6 MB para BMD.
        limit_=reserve<addressable?reserve:addressable;
        reserved=Align(limit_*stride);
        data=(char*)std::calloc(reserved,1);
        if(data) committed=reserved;
#else
        page=std::size_t(sysconf(_SC_PAGESIZE));
        limit_=addressable;
        reserved=Align(limit_*stride);
        data=(char*)mmap(nullptr,reserved,PROT_NONE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
        if(data==MAP_FAILED) data=nullptr;
#endif
        if(!data) throw std::bad_alloc();
    }
    ~Table() {
#ifdef _WIN32
        VirtualFree(data,0,MEM_RELEASE);
#elif defined(__DREAMCAST__)
        std::free(data);
#else
        munmap(data,reserved);
#endif
    }
    void Ensure(std::size_t requested) {
#ifdef __DREAMCAST__
        // Dynamic catalog assets intentionally live past the vanilla limits;
        // never abort the game - clamp to the reserved pool instead.
        if (requested > limit_) requested = limit_;
#else
        if (requested > limit_) throw std::length_error("Indice de recurso no representable en BMD/OBJECT (short)");
#endif
        if(requested<=count) return;
        std::size_t next=count?count*2:requested;
        if(next<requested) next=requested;if(next>limit_) next=limit_;
        const std::size_t bytes=Align(next*stride);
        if(bytes>committed) {
#ifdef _WIN32
            if(!VirtualAlloc(data+committed,bytes-committed,MEM_COMMIT,PAGE_READWRITE)) throw std::bad_alloc();
#elif defined(__DREAMCAST__)
            if(bytes>reserved) throw std::bad_alloc();   // ya reservado completo en el ctor
#else
            if(mprotect(data+committed,bytes-committed,PROT_READ|PROT_WRITE)) throw std::bad_alloc();
#endif
            committed=bytes;
        }
        count=next;
    }
    std::size_t Count() const { return count; }
    char* Base() { return data; }
    void Reset() { std::memset(data,0,count*stride); }
};
#ifdef __DREAMCAST__
// The dynamic catalog (ContentCatalog) allocates slots past the vanilla
// counts. Reserve extra headroom for it, but stay well below the 32768-slot
// full addressable arena (which would eat ~6 MB of BMD data alone).
constexpr std::size_t kDynamicHeadroom = 512;
#endif
Table& Models() { static Table table(sizeof(BMD), MODEL_MAX_VANILLA
#ifdef __DREAMCAST__
    + kDynamicHeadroom
#endif
);return table; }
Table& Textures() { static Table table(sizeof(void*)==4?0x38:0x40, BITMAP_MAX_VANILLA
#ifdef __DREAMCAST__
    + kDynamicHeadroom
#endif
);return table; }
}
void EnsureModels(std::size_t count) { Models().Ensure(count); }
void EnsureTextures(std::size_t count) { Textures().Ensure(count); }
std::size_t ModelCount() { return Models().Count(); }
std::size_t TextureCount() { return Textures().Count(); }
void* ModelBase() { EnsureModels(MODEL_MAX_VANILLA);return Models().Base(); }
char* TextureBase() { EnsureTextures(BITMAP_MAX_VANILLA);return Textures().Base(); }
void ResetModels() { ModelBase();Models().Reset(); }
}
