/* Joy 2001 */

#include "icio.h"

class IDE : public ICio {
private:
	uae_u8 addr;

public:
	IDE();
	virtual uae_u8 handleRead(uaecptr addr);
	virtual void handleWrite(uaecptr addr, uae_u8 value);
	virtual uae_u16 handleReadW(uaecptr addr);
	virtual void handleWriteW(uaecptr addr, uae_u16 value);
};
