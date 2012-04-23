
SET( SOURCES main)

FOREACH(src ${SOURCES})
	ADD_EXECUTABLE(${src} ${src}.cxx)
	TARGET_LINK_LIBRARIES(${src}  tub)
ENDFOREACH(src)


