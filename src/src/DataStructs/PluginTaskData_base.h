#ifndef DATASTRUCTS_PLUGINTASKDATA_BASE_H
#define DATASTRUCTS_PLUGINTASKDATA_BASE_H


#include "../../ESPEasy_common.h"

#include "../DataStructs/PluginStats.h"

#include "../DataTypes/PluginID.h"
#include "../DataTypes/TaskIndex.h"

// ==============================================
// Data used by instances of plugins.
// =============================================

// base class to be able to delete a data object from the array.
// N.B. in order to use this, a data object must inherit from this base class.
//      This is a compile time check.
struct PluginTaskData_base {
  PluginTaskData_base();
  virtual ~PluginTaskData_base();

  void initPluginStats(taskVarIndex_t taskVarIndex);
  void clearPluginStats(taskVarIndex_t taskVarIndex);

  // Called right after successful PLUGIN_READ to store task values
  void pushPluginStatsValues(struct EventStruct *event);

  bool plugin_write_base(struct EventStruct *event,
                         const String      & string);

  // We cannot use dynamic_cast, so we must keep track of the plugin ID to
  // perform checks on the casting.
  // This is also a check to only use these functions and not to insert pointers
  // at random in the Plugin_task_data array.
  pluginID_t _taskdata_pluginID = INVALID_PLUGIN_ID;

  // Array of pointers to PluginStats. One per task value.
  PluginStats *_plugin_stats[VARS_PER_TASK] = { nullptr, };
};

#endif // ifndef DATASTRUCTS_PLUGINTASKDATA_BASE_H
