
/**
 * Abstract base class for tools. Tools are the only way for the user to interact with the world.
 */
class Tool
{
public:
	
	
	
private:
	Vector3 translation;
};

class Forceps: public Tool
{
public:

private:
	void pinch();
	void release();
	
	ModelInstance forcepsModelInstance;
};

class Brush: public Tool
{
public:

private:
	void paint();
};