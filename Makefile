# Include debugging symbols (-g) or optimize code (-O or -O2 or -O3).
DEBUG=-g
#DEBUG=-O3

# PROFILING FLAG.
PROF=-pg

# LIBRARIES.
LIBS=-lGL -lglui -lGLEW -lglt \
  -lSDL -lSDL_image -lSDL_mixer -lSDL_ttf

# OBJECT FILES. (space-separated)
OBJS=SDLEngine.o World.o Stage.o \
     FlatSurface.o Parallelogram.o Triangle.o Player.o Shape.o \
     SkyBox.o Sound.o Sphere.o Utils.o Vector3d.o LineSegment.o \
	 external/geometryc.o FontManager.o HeadsUpDisplay.o \
	 Missile.o MissileParticleEffects.o ParticleEffects.o \
	 SmokeParticleEffects.o FireworksParticleEffects.o PowerupParticleEffects.o \
	 Powerup.o BigBall.o FastBall.o \
	 GameParameters.o menu/MenuManager.o  \
	 menu/Menu.o menu/NumPlayersMenu.o  menu/RootMenu.o \
	 menu/BallSelectionMenu.o menu/LevelSelectionMenu.o  \
	 menu/button/Button.o menu/button/TextButton.o \
	 menu/button/GLSphereButton.o LevelInfo.o StageMind.o \
	 menu/InstructionsMenu.o
	 
TARGET_OBJS=SphereWars.o
LEVELTESTER_OBJS=LevelTester.o

# EXECUTABLE NAME.
TARGET=spherewars
LEVELTESTER=leveltester

all: $(TARGET) $(LEVELTESTER)
prof: $(TARGET).prof

# LINKING.
$(TARGET): $(OBJS) $(TARGET_OBJS) Makefile
	$(CXX) -Wall $(DEBUG) -o $@ $(OBJS) $(TARGET_OBJS) -L$(BASEDIR)/lib \
	$(LIBS) 

$(TARGET).prof: $(OBJS) $(TARGET_OBJS) Makefile
	$(CXX) -Wall $(DEBUG) $(PROF) -o $@ $(OBJS) $(TARGET_OBJS) -L$(BASEDIR)/lib \
	$(LIBS) 

$(LEVELTESTER): $(OBJS) $(LEVELTESTER_OBJS) Makefile
	$(CXX) -Wall $(DEBUG) -o $@ $(OBJS) $(LEVELTESTER_OBJS) -L$(BASEDIR)/lib \
	$(LIBS) 


# COMPILATION.
.cpp.o:
	$(CXX) $(FLAGS) -Wall $(DEBUG) -I$(BASEDIR)/include/ -I/usr/include/GL -c -o $@ $*.cpp


# CLEANUP.
clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).prof $(LEVELTESTER)
