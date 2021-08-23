local dt = require "luci.cbi.datatypes"
local map = Map("projectiot", "projectiot Model")

local section = map:section(NamedSection, "projectiot_sct", "projectiot", "projectiot section")

local flag = section:option(Flag, "enable", "Enable", "Enable program")

local auth_orgid = section:option( Value, "auth_orgid", "Organization ID:")
auth_orgid.maxlength = 6
function auth_orgid.validate(self, value, section)
    if value and (#value == 6) then
        return value
      end
      return nil
    end

local auth_typeid = section:option( Value, "auth_typeid", "Type ID:")
function auth_typeid.validate(self, value, section)
    if value and (#value >= 1) and (#value <= 128) then
        return value
      end
      return nil
    end

local auth_deviceid = section:option( Value, "auth_deviceid", "Device ID:")
function auth_deviceid.validate(self, value, section)
    if value and (#value >= 1) and (#value <= 128) then
        return value
      end
      return nil
    end

local auth_authtoken = section:option( Value, "auth_authtoken", "Auth Token:")
auth_authtoken.datatype = "string"
auth_authtoken.maxlength = 32
function auth_authtoken.validate(self, value, section)
    if value and (#value >= 1) and (#value <= 128) then
        return value
      end
      return nil
    end


return map
