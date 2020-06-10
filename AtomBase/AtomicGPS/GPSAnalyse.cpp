#include "GPSAnalyse.h"

GPSAnalyse::GPSAnalyse()
{
    _GPS_Str.clear();
}


void GPSAnalyse::setSerialPtr(HardwareSerial &serial)
{
    _serial = &serial;
    _xSemaphore = xSemaphoreCreateMutex();
}

GPSAnalyse::~GPSAnalyse()
{

}

void GPSAnalyse::run(void *data)
{
    GPSReadBuff = (char*)calloc(1024,sizeof(char));
    Serial.println("GPS Task");
    while (1)
    {
        if (_serial->available())
        {
            
            memset(GPSReadBuff, 1024, sizeof(char));
            _serial->readBytes(GPSReadBuff, _serial->available());
            _GPS_Str.concat(GPSReadBuff);
            xSemaphoreTake(_xSemaphore, portMAX_DELAY);
            Analyse();
            xSemaphoreGive(_xSemaphore);
            delay(1);
        }
        else
        {
            delay(5);
        }
    }
}

void GPSAnalyse::upDate()
{
    xSemaphoreTake(_xSemaphore, portMAX_DELAY);
    memcpy(&s_GNRMC,&_s_GNRMC,sizeof(GNRMC_t));
    memcpy(&s_GNGAS,&_s_GNGAS,sizeof(GNGSA_t));
    memcpy(&s_GPGSV,&_s_GPGSV,sizeof(GPGSV_t));
    xSemaphoreGive(_xSemaphore);
}

void GPSAnalyse::AnaGPRMC(String str)
{
    str = str.substring(str.indexOf("GNRMC"), str.length());
    if (str.indexOf('*') != -1)
    {
        int indexsub = str.indexOf('*');
        String sumstr = str.substring(indexsub + 1, str.length());
        str = str.substring(0, indexsub);
        //Serial.println(sumstr);
    }

    int index = 0, last_index = 0, pamindex = 1;

    while (str.indexOf(',', index) != -1)
    {
        index = str.indexOf(',', index + 1);
        last_index = str.indexOf(',', index + 1);
        //Serial.printf("index:%d,%d\n", index, last_index);
        if (index != -1)
        {
            last_index = (last_index == -1) ? str.length() : last_index;
            if ((last_index - index) > 1)
            {
                String pamstr = str.substring(index + 1, last_index);
#ifdef DEBUG_GPS
                Serial.printf("%d:", pamindex);
                Serial.println(pamstr);
#endif
                _s_GNRMC.pamstr[pamindex] = pamstr;
            }
            else
            {
                _s_GNRMC.pamstr[pamindex].clear();
            }

            pamindex++;
        }
    }

    _s_GNRMC.Utc = _s_GNRMC.pamstr[1];
    if (_s_GNRMC.pamstr[2].length() != 0)
    {
        _s_GNRMC.State = _s_GNRMC.pamstr[2].charAt(0);
    }

    {
        _s_GNRMC.Latitude = _s_GNRMC.pamstr[3].substring(0, 2).toDouble() + _s_GNRMC.pamstr[3].substring(2, 4).toDouble() / 60 + _s_GNRMC.pamstr[3].substring(5, 9).toDouble() / 600000;
    }
    //_s_GNRMC.Latitude        = _s_GNRMC.pamstr[3].toDouble();
    _s_GNRMC.LatitudeMark = _s_GNRMC.pamstr[4].charAt(0);
    {
        _s_GNRMC.Longitude = _s_GNRMC.pamstr[5].substring(0, 3).toDouble() + _s_GNRMC.pamstr[5].substring(3, 5).toDouble() / 60 + _s_GNRMC.pamstr[5].substring(6, 10).toDouble() / 600000;
    }
    //_s_GNRMC.Longitude       = _s_GNRMC.pamstr[5].toDouble();
    _s_GNRMC.LongitudeMark = _s_GNRMC.pamstr[6].charAt(0);
    _s_GNRMC.TrackSpeed = _s_GNRMC.pamstr[7].toFloat();
    _s_GNRMC.TrackAngle = _s_GNRMC.pamstr[8].toFloat();
    _s_GNRMC.Date = _s_GNRMC.pamstr[9];
    _s_GNRMC.Magnetic = _s_GNRMC.pamstr[10].toFloat();
    _s_GNRMC.Declination = _s_GNRMC.pamstr[11].charAt(0);
    _s_GNRMC.mode = _s_GNRMC.pamstr[12].charAt(0);
}

void GPSAnalyse::AnaGNGAS(String str)
{
    str = str.substring(str.indexOf("GNGSA"), str.length());

    if (str.indexOf('*') != -1)
    {
        int indexsub = str.indexOf('*');
        String sumstr = str.substring(indexsub + 1, str.length());
        str = str.substring(0, indexsub);
        //Serial.println(sumstr);
    }

    int index = 0, last_index = 0, pamindex = 1;
    while (str.indexOf(',', index) != -1)
    {
        index = str.indexOf(',', index + 1);
        last_index = str.indexOf(',', index + 1);
        if (index != -1)
        {
            last_index = (last_index == -1) ? str.length() : last_index;
            if ((last_index - index) > 1)
            {
                String pamstr = str.substring(index + 1, last_index);
#ifdef DEBUG_GPS
                Serial.printf("%d:", pamindex);
                Serial.println(pamstr);
#endif
                _s_GNGAS.pamstr[pamindex] = pamstr;
            }
            else
            {
                _s_GNGAS.pamstr[pamindex].clear();
            }
            pamindex++;
        }
    }

    _s_GNGAS.mode2 = _s_GNGAS.pamstr[1].charAt(0);
    _s_GNGAS.mode1 = _s_GNGAS.pamstr[2].toInt();
    for (size_t i = 0; i < 12; i++)
    {
        _s_GNGAS.PINMap[i] = _s_GNGAS.pamstr[3 + i].toInt();
    }
    _s_GNGAS.PDOP = _s_GNGAS.pamstr[15].toFloat();
    _s_GNGAS.HDOP = _s_GNGAS.pamstr[16].toFloat();
    _s_GNGAS.VDOP = _s_GNGAS.pamstr[17].toFloat();
}

void GPSAnalyse::AnaGPGSV(String str)
{
    //Serial.println(str);
    str = str.substring(str.indexOf("GPGSV"), str.length());

    if (str.indexOf('*') != -1)
    {
        int indexsub = str.indexOf('*');
        String sumstr = str.substring(indexsub + 1, str.length());
        str = str.substring(0, indexsub);
        //Serial.println(sumstr);
    }

    int index = 0, last_index = 0, pamindex = 1;
    while (str.indexOf(',', index) != -1)
    {
        index = str.indexOf(',', index + 1);
        last_index = str.indexOf(',', index + 1);
        if (index != -1)
        {
            last_index = (last_index == -1) ? str.length() : last_index;
            if ((last_index - index) > 1)
            {
                String pamstr = str.substring(index + 1, last_index);
#ifdef DEBUG_GPS
                Serial.printf("%d:", pamindex);
                Serial.println(pamstr);
#endif
                _s_GPGSV.pamstr[pamindex] = pamstr;
            }
            else
            {
                _s_GPGSV.pamstr[pamindex].clear();
            }
            pamindex++;
        }
    }
    int SatelliteSize = (pamindex - 4) / 4;
    //Serial.printf("Number%d\n", SatelliteSize);

    _s_GPGSV.size = _s_GPGSV.pamstr[1].toInt();
    _s_GPGSV.Number = _s_GPGSV.pamstr[2].toInt();
    _s_GPGSV.SatelliteSize = _s_GPGSV.pamstr[3].toInt();

    if (_s_GPGSV.Number == 1)
    {
        for (size_t i = 0; i < 32; i++)
        {
            _s_GPGSV.Satellite[i].flag = false;
        }
    }
    for (size_t i = 0; i < SatelliteSize; i++)
    {
        int id = _s_GPGSV.pamstr[4 + (i * 4) + 0].toInt();
        if( id >= 32 ) continue;
        if(( 7 + (i * 4)) > 50 )
        {
            break;
        }
        _s_GPGSV.Satellite[id].elevation = _s_GPGSV.pamstr[4 + (i * 4) + 1].toInt();
        _s_GPGSV.Satellite[id].Azimuth = _s_GPGSV.pamstr[4 + (i * 4) + 2].toInt();
        _s_GPGSV.Satellite[id].SNR = (_s_GPGSV.pamstr[4 + (i * 4) + 3].length() == 0) ? -1 : _s_GPGSV.pamstr[4 + (i * 4) + 3].toInt();
        _s_GPGSV.Satellite[id].flag = true;
    }

    if (_s_GPGSV.Number == _s_GPGSV.size)
    {
        for (size_t i = 0; i < 32; i++)
        {
            if (_s_GPGSV.Satellite[i].flag == true)
            {
                Serial.printf("ID %d:%d,%d,%d\n", i, _s_GPGSV.Satellite[i].elevation, _s_GPGSV.Satellite[i].Azimuth, _s_GPGSV.Satellite[i].SNR);
            }
        }
    }
}

void GPSAnalyse::Analyse()
{
    while (_GPS_Str.indexOf('\r') != -1)
    {
        int index = _GPS_Str.indexOf('\r');
        String str = _GPS_Str.substring(0, index);
        _GPS_Str = _GPS_Str.substring(index + 3, _GPS_Str.length());

        str.trim();

        //Serial.println(str);

        if (str.indexOf("GNRMC") != -1)
        {
            AnaGPRMC(str);
        }
        else if (str.indexOf("GPVTG") != -1)
        {
        }
        else if (str.indexOf("GPGGA") != -1)
        {

        }
        else if (str.indexOf("GNGSA") != -1)
        {
            //Serial.print("GNGSA:");
            //AnaGNGAS(str);
        }
        else if (str.indexOf("GPGSV") != -1)
        {
            //Serial.print("GPGSV:");
            AnaGPGSV(str);
        }
        else if (str.indexOf("GPGLL") != -1)
        {
        }
    }
}
