use zbus::proxy;

#[proxy(
    default_service = "org.quartz.Session",
    default_path = "/org/quartz/Session",
    interface = "org.quartz.Session"
)]
pub trait Session {
    fn lock_screen(&self) -> zbus::Result<()>;
    fn request_logout(&self) -> zbus::Result<()>;
    fn request_reboot(&self) -> zbus::Result<()>;
    fn request_shutdown(&self) -> zbus::Result<()>;
    fn is_locked(&self) -> zbus::Result<bool>;
}
