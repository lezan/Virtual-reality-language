#include "bci.h"
#include "DarkGDK.h"

bci::bci(void) {

	connectionId=0;

	attentionLevel=0;
	blinkLevel=0;
	blink=false;
	meditationLevel=0;
	signalQuality=100;

	error=0;

	packetsRead=0;
	numPacketsRead=0;

	fogActive=false;
}

bci::~bci(void) {
}

void bci::getConnection(void) {

	connectionId = TG_GetNewConnectionId();
	error=TG_Connect( connectionId, "\\\\.\\COM1",TG_BAUD_57600, TG_STREAM_PACKETS );
}

void bci::enableBlinkDetection(void) {
	TG_EnableBlinkDetection(connectionId,1);
}
void bci::disableBlinkDetection(void) {
	TG_EnableBlinkDetection(connectionId,0);
}
void bci::readData(void) {
	packetsRead=TG_ReadPackets( connectionId, -1 );
	if(packetsRead>0) {
		numPacketsRead+=packetsRead;
		if(TG_GetValueStatus(connectionId, TG_DATA_ATTENTION)!=0) {
			//if (TG_GetValue(connectionId, TG_DATA_ATTENTION) != 0) {
				attentionLevel = (int)TG_GetValue(connectionId, TG_DATA_ATTENTION);
				//numPacketsAnalize++;
			//}
		}
		if(TG_GetValueStatus(connectionId, TG_DATA_MEDITATION)!=0) {
			//if (TG_GetValue(connectionId, TG_DATA_MEDITATION) != 0) {
				meditationLevel = (int)TG_GetValue(connectionId, TG_DATA_MEDITATION);
			//}
		}
		if(TG_GetValueStatus(connectionId, TG_DATA_BLINK_STRENGTH)!=0) {
			//if (TG_GetValue(connectionId, TG_DATA_BLINK_STRENGTH) != 0) {
				blinkLevel = (int)TG_GetValue(connectionId, TG_DATA_BLINK_STRENGTH);
				blink=true;
			//}
		}
		if(TG_GetValueStatus(connectionId, TG_DATA_POOR_SIGNAL)!=0) {
			//if (TG_GetValue(connectionId, TG_DATA_POOR_SIGNAL) != 0) {
				signalQuality = (int)TG_GetValue(connectionId, TG_DATA_POOR_SIGNAL);
			//}
		}		
	}
}
int bci::getLevelAttention(void) {
	return attentionLevel;
}
int bci::getLevelMeditation(void) {
	return meditationLevel;
}
int bci::getBlinkDetection(void) {
	return blinkLevel;
}
bool bci::checkBlinkDetection(void) {
	bool result=blink;
	blink=false;
	return result;
}
void bci::disconnected(void) {
	TG_FreeConnection(connectionId);
}

void bci::printBciInfo(void) {

	wsprintf(levelAttention, "Attention: %d", attentionLevel);		
	dbText(0,300,levelAttention);

	wsprintf(levelMeditation, "Meditation: %d", meditationLevel);		
	dbText(0,330,levelMeditation);

	wsprintf(levelBlink, "Blink: %d", blinkLevel);		
	dbText(0,360,levelBlink);

	wsprintf(blinkOn, "Blink: %d", blink );		
	dbText(0,390,blinkOn);
	
	wsprintf(totPacketsRead, "Pacchetti letti: %d", numPacketsRead);
	dbText(0,420,totPacketsRead);

	wsprintf(levelSignalQuality, "Noise: %d", signalQuality);
	dbText(0,450,levelSignalQuality);
}
/*void bci::updateFog(void) {
	packetsReadFog=TG_ReadPackets( connectionId, -1 );
	if(packetsReadFog>0) {
		numPacketsRead+=packetsReadFog;
		if(TG_GetValueStatus(connectionId, TG_DATA_ATTENTION)!=0) {
			if (TG_GetValue(connectionId, TG_DATA_ATTENTION) != 0) {
				attention = (int)TG_GetValue(connectionId, TG_DATA_ATTENTION);
				numPacketsAnalize++;
			}
		}
	}
        
    if(fogActive==true) {

        if(attention<10) {
			dbFogColor(141,141,141);
        }
        if((attention > 10) && (attention <= 20)) {
            dbFogDistance(50);
        }
        if((attention > 20) && (attention <= 30)) {
            dbFogDistance(70);
        }
        if((attention > 30) && (attention <= 40)) {
            dbFogDistance(90);
        }
        if((attention > 40) && (attention <= 50)) {
            dbFogDistance(110);
        }
        if((attention > 50) && (attention <= 60)) {
            dbFogDistance(150);
        }
        if((attention > 60) && (attention <= 70)) {
            dbFogDistance(180);
        }
        if((attention > 70) && (attention <= 80)) {
            dbFogDistance(210);
        }
        if((attention > 80) && (attention <= 90)) {
            dbFogDistance(300);
        }
        if((attention > 90) && (attention <= 100)) {
            dbFogDistance(5001);
        }
	}
}*/
void bci::checkgAvailableFog(void) {

	if(dbFogAvailable()==1) {
		dbFogOn();
		dbFogColor(141,141,141);
		dbFogDistance(80);
		fogActive=true;
	}
}

void bci::updateFog(void) {
	if(fogActive==true) {

		if(attentionLevel<10) {
			dbFogColor(141,141,141);
		}
		if((attentionLevel > 10) && (attentionLevel <= 20)) {
			dbFogDistance(50);
		}
		if((attentionLevel > 20) && (attentionLevel <= 30)) {
			dbFogDistance(70);
		}
		if((attentionLevel > 30) && (attentionLevel <= 40)) {
			dbFogDistance(90);
		}
		if((attentionLevel > 40) && (attentionLevel <= 50)) {
			dbFogDistance(110);
		}
		if((attentionLevel > 50) && (attentionLevel <= 60)) {
			dbFogDistance(150);
		}
		if((attentionLevel > 60) && (attentionLevel <= 70)) {
			dbFogDistance(180);
		}
		if((attentionLevel > 70) && (attentionLevel <= 80)) {
			dbFogDistance(210);
		}
		if((attentionLevel > 80) && (attentionLevel <= 90)) {
			dbFogDistance(300);
		}
		if((attentionLevel > 90) && (attentionLevel <= 100)) {
			dbFogDistance(5001);
		}
	}	
}
