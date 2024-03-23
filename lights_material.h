// properties struct
struct materials_t
{
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float shininess;
};

// light struct
struct light_t
{
	size_t name;
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float position[4];
};

//spotlight struct
struct spotlight_t
{
	size_t name;
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float position[4];
	float direction[3];
	float cutoff;
};


const materials_t brass = {
	{0.33f, 0.22f, 0.03f, 1.0f},
	{0.78f, 0.57f, 0.11f, 1.0f},
	{0.99f, 0.91f, 0.81f, 1.0f},
	27.8f
};

const materials_t shadow = {
	{0.0f, 0.0f, 0.0f, 0.5f},
	{0.0f, 0.0f, 0.0f, 0.5f},
	{0.0f, 0.0f, 0.0f, 0.5f},
	10.0f
};

const materials_t red_plastic = {
	{0.3f, 0.0f, 0.0f, 1.0f},
	{0.6f, 0.0f, 0.0f, 1.0f},
	{0.8f, 0.6f, 0.6f, 1.0f},
	32.0f
};

const materials_t white_shiny = {
	{0.1f, 0.1f, 0.1f, 1.0f},
	{0.5f, 0.5f, 0.5f, 1.0f},
	{0.5f, 0.5f, 0.5f, 1.0f},
	100.0f
};

const materials_t marble = {
	{0.5f, 0.5f, 0.5f, 1.0f},
	{0.5f, 0.4f, 0.2f, 1.0f},
	{0.4f, 0.3f, 0.2f, 1.0f},
	80.0f
};


const materials_t white = {
	{0.1f, 0.1f, 0.1f, 0.4f},
	{0.5f, 0.5f, 0.5f, 0.3f},
	{0.5f, 0.5f, 0.5f, 0.3f},
	0.0f
};

const materials_t white_matt = {
	{1.0f, 1.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
	0.0f
};

const materials_t menu = {
	{5.0f, 5.0f, 5.0f, 1.0f},
	{10.0f, 10.0f, 10.0f, 10.0f},
	{0.5f, 0.5f, 0.5f, 1.0f},
	200.0f
};

const materials_t mattblack = {
	{0.0f, 0.0f, 0.0f, 1.0f},
	{0.5f, 0.5f, 0.5f, 1.0f},
	{0.0f, 0.0f, 0.0f, 1.0f},
	60.0f
};



// void some predfined lights
spotlight_t topLight = {
	GL_LIGHT0,
	{0.5f, 0.5f, 0.5f, 1.0f},
	{2.0f, 2.0f, 2.0f, 2.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
	{0.0f, 290.0f, 0.0f, 1.0f},
	{0.0,1.0,0.0},
	40.0f
};

spotlight_t frontLight = {
	GL_LIGHT1,
	{0.5f, 0.5f, 0.5f, 1.0f},
	{0.8f, 0.8f, 0.8f, 1.0f},
	{0.05f, 0.05f, 0.05f, 1.0f},
	{0.0f, 0.0f, 600.0f, 1.0f},
	{0.0,0.0,-1.0},
	5.0f
};

spotlight_t redSpot = {
	GL_LIGHT2,
	{0.2f, 0.1f, 0.1f, 1.0f},
	{2.0f, 0.1f, 0.1f, 1.0f},
	{0.05f, 0.05f, 0.05f, 1.0f},
	{-280.0f, 280.0f, 0.0f, 1.0f},
	{0.0,-1.0,0.0},
	65.0f
};

spotlight_t yellowSpot = {
	GL_LIGHT3,
	{0.2f, 0.2f, 0.0f, 1.0f},
	{1.0f,1.0f, 0.0f, 1.0f},
	{0.05f, 0.05f, 0.05f, 1.0f},
	{280.0f, 280.0f, 0.0f, 1.0f},
	{0.0,-1.0,0.0},
	65.0f
};

spotlight_t redSpot2 = {
	GL_LIGHT4,
	{0.2f, 0.1f, 0.1f, 1.0f},
	{2.0f, 0.1f, 0.1f, 1.0f},
	{0.05f, 0.05f, 0.05f, 1.0f},
	{-280.0f, -280.0f, 0.0f, 1.0f},
	{0.0,1.0,0.0},
	65.0f
};

spotlight_t yellowSpot2 = {
	GL_LIGHT5,
	{0.2f, 0.2f, 0.0f, 1.0f},
	{1.0f,1.0f, 0.0f, 1.0f},
	{0.05f, 0.05f, 0.05f, 1.0f},
	{280.0f, -280.0f, 0.0f, 1.0f},
	{0.0,1.0,0.0},
	65.0f
};

spotlight_t redSpot3 = {
	GL_LIGHT6,
	{0.2f, 0.1f, 0.1f, 1.0f},
	{2.0f, 0.1f, 0.1f, 1.0f},
	{0.05f, 0.05f, 0.05f, 1.0f},
	{280.0f, 0.0f, 0.0f, 1.0f},
	{-1.0,0.0,0.0},
	80.0f
};


spotlight_t yellowSpot3 = {
	GL_LIGHT7,
	{0.2f, 0.2f, 0.0f, 1.0f},
	{1.0f,1.0f, 0.0f, 1.0f},
	{0.05f, 0.05f, 0.05f, 1.0f},
	{-280.0f, 0.0f, 0.0f, 1.0f},
	{1.0,0.0,0.0},
	80.0f
};

void set_material(const materials_t& mat)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat.ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat.diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat.specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat.shininess);
}

// set a given light
void set_light(const light_t& light)
{
	glLightfv(light.name, GL_AMBIENT, light.ambient);
	glLightfv(light.name, GL_DIFFUSE, light.diffuse);
	glLightfv(light.name, GL_SPECULAR, light.specular);
	glLightfv(light.name, GL_POSITION, light.position);


	glEnable(light.name);
}

//turn off a given light
void switchoff_light(const light_t& light)
{
	glDisable(light.name);
}

//turn off a given light
void switchoff_light(const spotlight_t& light)
{
	glDisable(light.name);
}

//turn on a given light
void turnon_light(const light_t& light){
	glEnable(light.name);
}

//turn on a given spot light
void turnon_light(const spotlight_t& light){
	glEnable(light.name);
}

// set a given light
void set_light(const spotlight_t& light)
{
	glLightfv(light.name, GL_AMBIENT, light.ambient);
	glLightfv(light.name, GL_DIFFUSE, light.diffuse);
	glLightfv(light.name, GL_SPECULAR, light.specular);
	glLightfv(light.name, GL_POSITION, light.position);
	glLightfv(light.name, GL_SPOT_DIRECTION, light.direction);
	glLightf(light.name, GL_SPOT_CUTOFF, light.cutoff);

	glEnable(light.name);
}
