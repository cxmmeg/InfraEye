set(COMPONENT_SRCS 	"app_main.c"
					"app_display.c"
					"module_IRsensor.h")
					
set(COMPONENT_ADD_INCLUDEDIRS ".")
set(COMPONENT_SRCDIRS ".")
set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_EMBED_FILES image.jpg)

register_component()
