all: main.cpp run

LDFLAGS	   = -ljpeg

CLASSDIR   = Classes
TOOLDIR	   = Tools
SOURCES    = main.cpp\
	     ${CLASSDIR}/SPHERE.cpp\
	     ${CLASSDIR}/LIGHT.cpp\
	     ${CLASSDIR}/POINT.cpp\
	     ${CLASSDIR}/ACTOR.cpp\
	     ${CLASSDIR}/CUSTOMACTOR.cpp\
	     ${CLASSDIR}/TRIANGLE.cpp\
	     ${CLASSDIR}/CYLINDER.cpp\
	     ${TOOLDIR}/RAYTRACER.cpp\
	     ${TOOLDIR}/JRNG.cpp\
	     ${TOOLDIR}/DISPLAYSKELETON.cpp\
	     ${TOOLDIR}/MOTION.cpp\
	     ${TOOLDIR}/SKELETON.cpp

OBJECTS    = $(SOURCES:.cpp=.o)

.cpp.o:
	mpicxx -g -O3 -w -c $< -o $@

run: ${OBJECTS}
	mpicxx -g $(OBJECTS) $(LDFLAGS) -o $@

clean:
	rm -f *.o ${CLASSDIR}/*.o ${TOOLDIR}/*.o run

