/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cutils/properties.h>
#include "StartPropertySetThread.h"
#ifdef QUICKBOOT
#include <unistd.h>
#include <fcntl.h>
#endif

namespace android {

StartPropertySetThread::StartPropertySetThread(bool timestampPropertyValue):
        Thread(false), mTimestampPropertyValue(timestampPropertyValue) {}

status_t StartPropertySetThread::Start() {
    return run("SurfaceFlinger::StartPropertySetThread", PRIORITY_NORMAL);
}

bool StartPropertySetThread::threadLoop() {
    // Set property service.sf.present_timestamp, consumer need check its readiness
    property_set(kTimestampProperty, mTimestampPropertyValue ? "1" : "0");
    // Clear BootAnimation exit flag
    property_set("service.bootanim.exit", "0");
    // Start BootAnimation if not started
#ifdef QUICKBOOT
    int fdDmesg = open("/dev/kmsg", O_WRONLY);
    if (fdDmesg > 0) {
        static const char _message[] = { '<', '0', '1', '>',
            'R', 'u', 'n', ' ', 'B', 'o', 'o', 't', 'a', 'n', 'i', 'm', '\n' };
        write(fdDmesg, _message, sizeof(_message));
        close(fdDmesg);
    }
#endif
    property_set("ctl.start", "bootanim");
    // Exit immediately
    return false;
}

} // namespace android
