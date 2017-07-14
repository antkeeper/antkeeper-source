
enum class ToolContext
{
	BRUSH_PAINT,
	LENS_FOCUS,
	FORCEPS_PINCH,
	FORCEPS_RELEASE,
	FORCEPS_PICK_UP,
	FORCEPS_PUT_DOWN
};

class Tool
{
public:

	ToolContext gatherContext();
};