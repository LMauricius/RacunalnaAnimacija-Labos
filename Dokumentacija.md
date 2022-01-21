# Dokumentacija


## Izgradnja programa

Program je rađen pomoću pogonskog softwarea Ogre3D te je potrebno preuzeti i instalirati Ogre3D (https://www.ogre3d.org/download/sdk/sdk-ogre-next), verziju 2.3. Rad je testiran na Linuxu baziranom na Ubuntu-u, no trebao bi raditi i na drugim sustavima. Korišten je sustav za izgradnju CMake (https://cmake.org/), te je tijekom konfiguriranja projekta potrebno postaviti CMake-ove varijable OGRE_SOURCE i OGRE_DEPENDENCIES_DIR. Ako se koristi VSCode, najlakše je u datoteku settings.json dodati isječak: 
```json
"cmake.configureSettings":
{
    "OGRE_DEPENDENCIES_DIR": ".../Ogre/ogre-next-deps/build/ogredeps",
    "OGRE_SOURCE": ".../Ogre/ogre-next"
},
```

Nakon kompiliranja program bi trebao biti spreman za pokretanje. Moći ćete namjestiti nekoliko grafičkih postavki prilikom pokretanja.

## Opis rada

Rad demonstrira jednostavan sustav za animaciju koji podržava unaprijednu i inverznu kinematiku. Sustav se može jednostavno postaviti pomoću .json datoteke te se koristi pozivanjem nekoliko metoda. Animacije s unaprijednom kinematikom koriste sustav ugrađen u Ogre3D, dok su animacije s inverznom kinematikom implementirane pomoću algoritma FABRIK. FABRIK sam odabrao zato što njegovi rezultati nalikuju na prirodne pokrete te zbog njegove konceptualne jednostavnosti. Sustav animacije nudi mogućnost glatkih prijelaza između animacija i istovremenu primjenu više animacije pomoću sustava utora (slot) te mogućnost dijeljenja timera između animacija kako bi se više animacija moglo sinkronizirati.


## Koncepti

### Utori (slots)
Utori omogućuju istovremenu primjenu više animacija, kao i međusobno poništavanje animacija uz glatke prijelaze. Svaka animacija ima postavljen utor, a ako utor nije specificiran u postavkama, stvara se novi samo za tu animaciju. Utori zapisuju trenutne animacije pomoću stoga. Kad se animacija pokrene, dodaje se na vrh stoga svog utora te povećavanjem težine jedne animacije smanjuje se izvedena težina animacija ispod nje. Kad izvedena težina neke animacije padne na 0, ona se uklanja sa stoga. Taj sustav omogućuje glatke prijelaze između više od 2 animacije, ako se nova pusti prije nego je prošli prijelaz završio.

### Brojač (timer)
Brojač je zaseban objekt od animacije, koji se koristi kako bi se računala trenutna vremenska pozicija u unaprijednim skeletalnim animacijama. Više animacija može imati isti brojač, čime se postiže sinkronizacija više animacija. 