#include <assert.h>

#include "runtime/core/meta/json.h"
#include "runtime/core/meta/reflection/reflection.h"
#include "runtime/core/meta/reflection/reflection_register.h"
#include "runtime/core/meta/serializer/serializer.h"

#include "generated/reflection/all_reflection.h"
#include "generated/serializer/all_serializer.ipp"

namespace MiniEngine
{
    namespace Reflection
    {
        void TypeMetaRegister::metaUnregister() { TypeMetaRegisterinterface::unregisterAll(); }
    } // namespace Reflection
} // namespace MiniEngine
