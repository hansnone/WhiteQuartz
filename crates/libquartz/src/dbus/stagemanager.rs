use zbus::proxy;

#[proxy(
    default_service = "org.kde.KWin",
    default_path = "/org/quartz/Compositor/StageManager",
    interface = "org.quartz.Compositor.StageManager"
)]
pub trait StageManager {
    fn set_enabled(&self, enabled: bool) -> zbus::Result<()>;
    fn is_enabled(&self) -> zbus::Result<bool>;
    fn activate_stage_group(&self, group_id: &str) -> zbus::Result<()>;
    fn add_window_to_current_stage(&self, window_internal_id: &str) -> zbus::Result<()>;
}
