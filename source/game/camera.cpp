#include "game/camera.h"

#include "common/utils.h"

namespace game {

u32 Camera::ChangeMode(CameraMode mode, u32 unknown) {
  return rst::util::GetPointer<u32(Camera*, CameraMode, u32)>(0x18B31C)(this, mode, unknown);
}

}  // namespace game
