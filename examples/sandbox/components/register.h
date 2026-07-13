#ifndef SANDBOX_COMPONENTS_REGISTER_H
#define SANDBOX_COMPONENTS_REGISTER_H

#include <anchorpoint/scene/scene_loader.h>

namespace sandbox {

// Registers scene loaders for the sandbox's custom components so they can be
// placed in .scene files (keyed by their struct name). Keeps main() free of
// per-component boilerplate.
void RegisterComponents(ap::SceneLoader& scenes);

} // namespace sandbox

#endif //SANDBOX_COMPONENTS_REGISTER_H
