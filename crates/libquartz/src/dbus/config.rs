use zbus::proxy;

#[proxy(
    default_service = "org.quartz.Config",
    default_path = "/org/quartz/Config",
    interface = "org.quartz.Config"
)]
pub trait Config {
    fn get_config(&self) -> zbus::Result<String>;
    fn set_config(&self, json: &str) -> zbus::Result<bool>;

    fn get_stage_manager_enabled(&self) -> zbus::Result<bool>;
    fn set_stage_manager_enabled(&self, enabled: bool) -> zbus::Result<bool>;

    fn get_appearance_mode(&self) -> zbus::Result<String>;
    fn set_appearance_mode(&self, mode: &str) -> zbus::Result<bool>;

    fn get_theme(&self) -> zbus::Result<String>;
    fn set_theme(&self, theme: &str) -> zbus::Result<bool>;

    fn get_icon_theme(&self) -> zbus::Result<String>;
    fn set_icon_theme(&self, icon_theme: &str) -> zbus::Result<bool>;

    #[zbus(signal)]
    fn config_changed(&self, json: &str) -> zbus::Result<()>;
}
