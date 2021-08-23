include $(TOPDIR)/rules.mk

PKG_NAME:=projectiot
PKG_RELEASE:=1
PKG_VERSION:=1.0.12

include $(INCLUDE_DIR)/package.mk

define Package/projectiot
	CATEGORY:=Base system
	TITLE:=projectiot
	DEPENDS:=+libwatson-iot-c +libubus +libubox +libblobmsg-json +libuci
endef

define Package/projectiot/description
	Project using IOT.
endef

define Package/projectiot/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/projectiot $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/projectiot.init $(1)/etc/init.d/$(PKG_NAME)
	$(INSTALL_CONF) ./files/projectiot.config $(1)/etc/config/$(PKG_NAME)
endef

$(eval $(call BuildPackage,projectiot))