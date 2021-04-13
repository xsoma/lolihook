#include "..\hooks.hpp"


static const DemoPlaybackParameters* __stdcall getDemoPlaybackParameters() noexcept
{
    const auto params = engine_hook->get_func_address <DemoPlaybackParameters>(218);


    if (params && config->misc.revealSuspect && *static_cast<std::uint64_t*>(_ReturnAddress()) != 0x79801F74C985C88B) { // client.dll : 8B C8 85 C9 74 1F 80 79 10 00 , there game decides whether to show overwatch panel
        static DemoPlaybackParameters customParams;
        customParams = *params;
        customParams.anonymousPlayerIdentity = false;
        return &customParams;
    }

    return params;
}
