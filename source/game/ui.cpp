#include "game/ui.h"

#include <string_view>

#include "common/context.h"
#include "common/debug.h"
#include "common/utils.h"
#include "game/context.h"
#include "game/static_context.h"
#include "game/ui/screens/screen.h"

namespace game::ui {

ScreenContext& GetScreenContext() {
  return *rst::util::GetPointer<ScreenContext>(0x72F5BC);
}

Screen* GetScreen(ScreenType screen) {
  switch (screen) {
  case ScreenType::Black:
    return *rst::util::GetPointer<Screen*>(0x656B00);
  case ScreenType::Main:
    return *rst::util::GetPointer<Screen*>(0x656B04);
  case ScreenType::Schedule:
    return *rst::util::GetPointer<Screen*>(0x656B08);
  case ScreenType::Quest:
    return *rst::util::GetPointer<Screen*>(0x656B0C);
  case ScreenType::Items:
    return *rst::util::GetPointer<Screen*>(0x656B10);
  case ScreenType::Map:
    return *rst::util::GetPointer<Screen*>(0x656B14);
  case ScreenType::Masks:
    return *rst::util::GetPointer<Screen*>(0x656B18);
  case ScreenType::Ocarina:
    return *rst::util::GetPointer<Screen*>(0x656B1C);
  }
  return nullptr;  // should be unreachable
}

bool OpenScreen(ScreenType screen) {
  GlobalContext* gctx = rst::GetContext().gctx;
  ScreenContext& ui_ctx = GetScreenContext();

  if (!gctx || gctx->type != StateType::Play)
    return false;

  ui_ctx.new_screen = GetScreen(screen);
  gctx->ui_menu_state = UiMenuState::Opening;
  gctx->enable_letterbox = false;

  if (GetStaticContext().field_D38) {
    GetStaticContext().field_D38 = 0;
    gctx->field_CAB0 = 0xA0A0A000;
  }
  if (!gctx->field_CAAD)
    gctx->field_CAB0 = 0;

  if (gctx->some_fn2 && !gctx->some_fn2(&gctx->some_ptr) && gctx->some_fn1)
    gctx->some_fn1(&gctx->some_ptr, 0);

  return true;
}

bool CheckCurrentScreen(ScreenType screen) {
  return GetScreenContext().active_screen == GetScreen(screen);
}

void LayoutBase::calc(float speed) {
  // ends up calling this->Calc(...) with a bunch of global arguments
  rst::util::GetPointer<void(LayoutBase*, float)>(0x161AE8)(this, speed);
}

AnimPlayer* LayoutBase::GetAnimPlayer(std::string_view name) const {
  for (auto* player : players) {
    if (player->GetName() == name)
      return player;
  }
  return nullptr;
}

Widget* LayoutBase::GetWidget(std::string_view name) {
  return root_widget.GetWidget(name);
}

Pane* LayoutBase::GetPane(std::string_view name) const {
  for (auto* pane : panes) {
    if (pane->GetName() == name)
      return pane;
  }
  return nullptr;
}

WidgetType Widget::GetType() const {
  if (widgets.data)
    return WidgetType::Group;
  if (layout)
    return WidgetType::Layout;
  if (main_widget_idx != 0xffff)
    return WidgetType::MainWidget;
  return WidgetType::Pane;
}

Widget* Widget::GetWidget(std::string_view name) {
  if (this->name == name)
    return this;

  // Search for the widget recursively.
  for (auto* widget : widgets) {
    if (auto* found = widget->GetWidget(name))
      return found;
  }

  return nullptr;
}

void Widget::PrintDebug() {
  auto& tx = GetPos().translate;
  rst::util::Print("%p %s tx=(%f %f %f) mtx=(%f %f %f) %08lx %08lx %f", this, GetName(), tx.x, tx.y,
                   tx.z, mtx(0, 3), mtx(1, 3), mtx(2, 3), GetPos().flags.flags,
                   GetPos().active_flags.flags, vec4(3));
}

void AnimPlayer::Play(Anim* anim, float frame, bool x9) {
  m_anim = anim;
  m_x9 = x9;
  SetFrame(frame);
  if (m_anim)
    m_playing = true;
  m_xa = true;
}

void AnimPlayer::SetAnimAndPause(Anim* anim, float frame) {
  m_anim = anim;
  SetFrame(frame);
  m_playing = false;
  m_xa = true;
}

void AnimPlayer::ChangeAnim(Anim* anim) {
  m_anim = anim;
  SetFrame(m_frame);
  if (!m_anim)
    m_playing = false;
  m_xa = true;
}

void AnimPlayer::Stop() {
  SetFrame(0.0);
  m_playing = false;
  m_xa = true;
}

void AnimPlayer::Reset() {
  m_anim = nullptr;
  m_x9 = 0;
  SetFrame(0.0);
  m_playing = false;
  m_speed = 1.0;
  m_xa = true;
}

void AnimPlayer::SetFrame(float frame) {
  rst::util::GetPointer<void(AnimPlayer*, float)>(0x192AE0)(this, frame);
}

Anim* LayoutClass::GetAnim(std::string_view name) const {
  for (auto& anim : GetAnims()) {
    if (anim.GetName() == name)
      return &anim;
  }
  return nullptr;
}

LayoutMgr& LayoutMgr::Instance() {
  return rst::util::GetInstance<LayoutMgr>(std::tuple{0x7CDC9C}, std::tuple{0x6B0AFC},
                                           std::tuple{0x16963C});
}

void LayoutMgr::FreeLayout(Layout* layout) {
  rst::util::GetPointer<void(LayoutMgr&, Layout*)>(0x169634)(*this, layout);
}

Layout* LayoutMgr::MakeLayout(int file, int x) {
  return rst::util::GetPointer<Layout*(LayoutMgr&, int, int)>(0x16962C)(*this, file, x);
}

Layout* LayoutMgr::MakeLayout(const char* name) {
  int file = Project::Instance().GetLayoutId(name);
  return MakeLayout(file);
}

Project& Project::Instance() {
  return *rst::util::GetPointer<Project>(0x7CDCA0);
}

int Project::GetLayoutId(const char* name) {
  return rst::util::GetPointer<int(Project&, const char*)>(0x16A014)(*this, name);
}

int Project::GetPackageId(const char* name) {
  return rst::util::GetPointer<int(Project&, const char*)>(0x15A86C)(*this, name);
}

bool Project::LoadPackage(int id, bool x) {
  return rst::util::GetPointer<bool(Project&, int, bool)>(0x15141C)(*this, id, x);
}

bool Project::UnloadPackage(int id) {
  return rst::util::GetPointer<bool(Project&, int)>(0x161CB4)(*this, id);
}

bool Project::IsLoading() const {
  return rst::util::GetPointer<bool(const Project&)>(0x180190)(*this);
}

LayoutDrawMgr& LayoutDrawMgr::Instance() {
  return *rst::util::GetPointer<LayoutDrawMgr*()>(0x169824)();
}

void LayoutDrawMgr::ControlLayout(Layout* layout, int a, int b) {
  rst::util::GetPointer<void(LayoutDrawMgr*, Layout*, int, int)>(0x17EC58)(this, layout, a, b);
}

}  // namespace game::ui
