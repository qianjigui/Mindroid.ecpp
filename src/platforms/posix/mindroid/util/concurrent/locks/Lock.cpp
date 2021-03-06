/*
 * Copyright (C) 2011 Daniel Himmelein
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

#include <stdlib.h>
#include "mindroid/util/concurrent/locks/Lock.h"

namespace mindroid {

#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
pthread_mutex_t Lock::sLock = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
#else
pthread_mutex_t Lock::sLock = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#endif

bool Lock::lock() {
    return (pthread_mutex_lock(&sLock) == 0);
}

void Lock::unlock() {
    pthread_mutex_unlock(&sLock);
}

} /* namespace mindroid */
