#pragma once

#include "config.h"

/** Data Storage for plant logging
 * On the files, the first line includes plant name, start date, growth plan
 * On subsequent lines, time, moisture, temperature, light, and other misc data
 */


bool storedata(String filename, String data);

String retrievedata(String filename);

bool createfile(String filename);

void storemoisturesettings();

String consolidatedata();

String initplantfileheaders();

bool storeplantdata(String filename);

bool setupplantdata(String filename);