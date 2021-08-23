module("luci.controller.projectiot_controller", package.seeall)

function index()
	entry({"admin", "services", "projectiot_model"}, cbi("projectiot_model"), _("projectiot Model"),105)
end
