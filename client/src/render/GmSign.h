#pragma once

// Signo sobre la cabeza de los GameMasters (5.2: CSParts con MODEL_WEBZEN_MARK).
// El 0.97k no lo tiene; el nivel lo manda el server en el 0x12 (protocolo 13).
namespace GmSign {

// Nivel de GameMaster.txt del personaje: 0 = jugador, 1 = GM, 2 = administrador.
void SetLevel(const void* character, int level);

// Nivel del héroe.  Va aparte porque el server lo manda en el 0x12 propio, que
// llega antes de que el héroe exista (lo procesa el pump de OpenWorld).
void SetHeroLevel(int level);

// Dibuja el signo si el personaje tiene nivel.  Va en el camino de dibujo de
// RenderCharacter (después del cuerpo), no en Equipment_Tick: ese sólo registra.
void Render(void* character);

}
