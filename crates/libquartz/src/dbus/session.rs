use zbus::proxy;

#[proxy(
    default_service = "org.quartz.Session",
    default_path = "/org/quartz/Session",
    interface = "org.quartz.Session"
)]
pub trait Session {
    // Control de energía y sesión
    fn lock_screen(&self) -> zbus::Result<()>;
    fn request_logout(&self) -> zbus::Result<()>;
    fn request_reboot(&self) -> zbus::Result<()>;
    fn request_shutdown(&self) -> zbus::Result<()>;
    fn is_locked(&self) -> zbus::Result<bool>;

    // Ciclo de vida de aplicaciones y retención (Ctrl+W vs Ctrl+Q)
    fn on_window_opened(&self, app_id: &str, name: &str) -> zbus::Result<()>;
    fn on_window_closed(&self, app_id: &str) -> zbus::Result<bool>;
    fn quit_application(&self, app_id: &str) -> zbus::Result<()>;
    fn is_app_running(&self, app_id: &str) -> zbus::Result<bool>;
}
