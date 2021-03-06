#include "simcc/inner_pre.h"
#include "simcc/file_util.h"

#include "inherited_conf_json.h"
#include "json_cast.h"

namespace simcc {
namespace json {

static const string kInheritedFrom = "inherited_from";

// @brief 得到该路径的真实路径
// @param[in] - const string & inherited_from_file_path
// @return - string
static string GetRealPath(const string& parent_file_path, const string& inherited_from_file_path) {
    if (simcc::FileUtil::IsAbsolutePath(inherited_from_file_path)) {
        return inherited_from_file_path;
    }

    string dir, filename;
    simcc::FileUtil::SplitFileName(parent_file_path, filename, dir);
    return simcc::FileUtil::Join(dir, inherited_from_file_path);
}

JSONObjectPtr InheritedConfJSONObject::Parse(const string& json_file_path) {
    ObjectPtr jbase = JSONParser::LoadFile(json_file_path);
    if (!jbase || !jbase->IsTypeOf(kJSONObject)) {
        return JSONObjectPtr();
    }
    assert(jbase && jbase->IsTypeOf(kJSONObject));
    JSONObject* jconf = cast<JSONObject>(jbase);
    string inherited_from_file = jconf->GetString(kInheritedFrom);
    if (inherited_from_file.empty()) {
        return jconf;
    }
    inherited_from_file = GetRealPath(json_file_path, inherited_from_file);
    JSONObjectPtr jconf_inherited = Parse(inherited_from_file); // recursively
    jconf->Merge(jconf_inherited.get(), false);
    return jconf;
}
}
}