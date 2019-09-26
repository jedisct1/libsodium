// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
// Modified by Dan Kouba <dan.kouba@particle.io>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "rng_hal.h"
#include "randombytes_internal.h"

static const char *randombytes_particle_implementation_name(void)
{
    return "Particle";
}

static void particle_fill_random(void* data, size_t size) {
    for (uint32_t* ptr = (uint32_t*)data; (uint32_t*)ptr < (uint32_t*)(data + size); ptr++) {
        *ptr = HAL_RNG_GetRandomNumber();
    }
}

/*
  Plug the Particle hardware RNG into libsodium's custom RNG support, as per
  https://download.libsodium.org/doc/advanced/custom_rng.html
  Note that this RNG is selected by default (see randombytes_default.h), so there
  is no need to call randombytes_set_implementation().
*/
const struct randombytes_implementation randombytes_particle_implementation = {
    .implementation_name = randombytes_particle_implementation_name,
    .random = HAL_RNG_GetRandomNumber,
    .stir = NULL,
    .uniform = NULL,
    .buf = particle_fill_random,
    .close = NULL,
};