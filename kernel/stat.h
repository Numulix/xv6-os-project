#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Device

// File owner
#define U_R 0400	// Read
#define U_W 0200	// Write
#define U_X 0100	// Execute

// Group
#define G_R 0040
#define G_W 0020
#define G_X 0010

// Other
#define O_R 0004
#define O_W 0002
#define O_X 0001

struct stat {
	short type;  // Type of file
	int dev;     // File system's disk device
	uint ino;    // Inode number
	short nlink; // Number of links to file
	uint size;   // Size of file in bytes
	uint mode;
	uint uid;
	uint gid;
};
