#pragma once

native X_EnableOpcodeLimit                          takes boolean enable returns nothing
native X_EnableOpcodeCounter                        takes boolean enable returns nothing
native X_SetOpcodeBehavior                          takes boolean enablelimit, boolean enablecounter returns nothing
