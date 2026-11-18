#include "stdafx.h"
#include "platform/GuardedOperation.h"
#include "platform/IFileSystem.h"
#include "assets/data/ContentCatalog.h"
#include "game/item/ItemDefines.h"
#include "game/item/Item_ServerValue.h"

// RenderItemInfo.cpp  @0x004C4650 / 0x004C8D70
// Populates the item info tooltip string buffer (TextList, stride 100, ~0x18 slots)
// and the color-flag array (TextListColor). Called from inventory/shop tooltip draw path.

// SEH wrapper (desviación): RenderItemInfo / RenderRepairInfo envuelven el
// port en __try/__except para que un AV interno (p.ej. snprintf con `%s`
// sobre un puntero de nombre corrupto) no mate el proceso: el tooltip
// simplemente no aparece ese frame.
extern "C" void __cdecl RenderRepairInfo_impl(void*, int, void*);
extern "C" void DbgLogPublic(const char* msg);
char* __cdecl GetMapName(int iMap);

static bool IsRenderRepairInfoBlocked(short type)
{
    if ((type >= 416 + 103 && type <= 416 + 107) ||
        (type >= 416 + 109 && type <= 416 + 115) ||
        type == 416 + 121 ||
        type == 448 + 112 || type == 448 + 113 ||
        type == 448 + 120 || type == 448 + 121 || type == 448 + 122 ||
        type == 448 + 123 || type == 448 + 124 ||
        type == 448 + 133 ||
        (type >= 448 + 114 && type <= 448 + 119) ||
        (type >= 448 + 126 && type <= 448 + 132) ||
        (type >= 448 + 134 && type <= 448 + 139) ||
        (type >= 384 + 130 && type <= 384 + 135)) {
        return true;
    }
    return false;
}

// Copia `src` a `dst` colapsando "%%" -> "%".
// Equivale a `sprintf(dst, src)` con cero argumentos, que es lo que hace el
// binario con las lineas de GlobalText que no llevan parametros, pero sin el
// riesgo de que un %d perdido en los datos lea la pila.
static void CopyCollapsingPercent(char* dst, size_t dstSize, const char* src)
{
    if (!dst || dstSize == 0) return;
    if (!src) { dst[0] = 0; return; }
    size_t o = 0;
    for (size_t i = 0; src[i] && o + 1 < dstSize; ++i) {
        if (src[i] == '%' && src[i + 1] == '%') ++i;
        dst[o++] = src[i];
    }
    dst[o] = 0;
}

static bool GetInventorySpecialOptionText(short type, BYTE option, BYTE value, int mana, char* dst, size_t dstSize)
{
    if (!dst || dstSize == 0)
        return false;

    dst[0] = '\0';

    switch (option) {
    case 18:
        snprintf(dst, dstSize, GlobalText[80], mana);
        return true;
    case 19:
        snprintf(dst, dstSize, GlobalText[81], mana);
        return true;
    case 20:
        snprintf(dst, dstSize, GlobalText[82], mana);
        return true;
    case 21:
        snprintf(dst, dstSize, GlobalText[83], mana);
        return true;
    case 22:
        snprintf(dst, dstSize, GlobalText[84], mana);
        return true;
    case 23:
        snprintf(dst, dstSize, GlobalText[85], mana);
        return true;
    case 24:
        snprintf(dst, dstSize, GlobalText[86], mana);
        return true;
    case 49:
        snprintf(dst, dstSize, GlobalText[745], mana);
        return true;
    case 56:
        snprintf(dst, dstSize, GlobalText[98], mana);
        return true;
    case 60:
        snprintf(dst, dstSize, GlobalText[88], value);
        return true;
    case 61:
        snprintf(dst, dstSize, GlobalText[89], value);
        return true;
    case 62:
        snprintf(dst, dstSize, GlobalText[90], value);
        return true;
    case 63:
        snprintf(dst, dstSize, GlobalText[91], value);
        return true;
    case 64:
        // GlobalText[87] = "Suerte (tasa de exito de Jewel of Soul +25%%)".
        // El %% es un ESCAPE: el original la pasa a sprintf como cadena de
        // formato SIN argumentos, y ahi %% colapsa a %.  Copiarla con "%s"
        // (como se hacia aca) dejaba el doble porcentaje visible: "+25%%".
        CopyCollapsingPercent(dst, dstSize, GlobalText[87]);
        return true;
    case 65:
        if (!(ITEM_HELPER_BASE + 14 <= type && type <= ITEM_HELPER_BASE + 18)) {
            snprintf(dst, dstSize, GlobalText[92], value);
            return true;
        }
        return false;
    case 66:
        snprintf(dst, dstSize, GlobalText[622], 4);
        return true;
    case 67:
        snprintf(dst, dstSize, GlobalText[623], 4);
        return true;
    case 68:
        snprintf(dst, dstSize, GlobalText[624], 4);
        return true;
    case 69:
        snprintf(dst, dstSize, GlobalText[625], 5);
        return true;
    case 70:
        snprintf(dst, dstSize, GlobalText[626], 10);
        return true;
    case 71:
        snprintf(dst, dstSize, GlobalText[627], 30);
        return true;
    case 72:
        snprintf(dst, dstSize, GlobalText[628], 10);
        return true;
    case 73:
        snprintf(dst, dstSize, GlobalText[629], 20);
        return true;
    case 74:
        snprintf(dst, dstSize, GlobalText[630], 2);
        return true;
    case 75:
        snprintf(dst, dstSize, GlobalText[631], 20);
        return true;
    case 76:
        snprintf(dst, dstSize, GlobalText[632], 2);
        return true;
    case 77:
        snprintf(dst, dstSize, GlobalText[633], value);
        return true;
    case 78:
        snprintf(dst, dstSize, GlobalText[634], 8);
        return true;
    case 79:
        snprintf(dst, dstSize, GlobalText[635], 8);
        return true;
    case 80:
        snprintf(dst, dstSize, GlobalText[740], value);
        return true;
    case 81:
        snprintf(dst, dstSize, GlobalText[741], value);
        return true;
    case 82:
        snprintf(dst, dstSize, GlobalText[742], value);
        return true;
    case 83:
        snprintf(dst, dstSize, GlobalText[743], value);
        return true;
    case 84:
        snprintf(dst, dstSize, GlobalText[744], value);
        return true;
    case 90:
        snprintf(dst, dstSize, "%s", GlobalText[746]);
        return true;
    default:
        return false;
    }
}

// IDA: RenderItemInfo (0x004C4650)
// ═════════════════════════════════════════════════════════════════════════════
// RenderItemInfo — port fiel de IDA (0x004C4650).
//
// Reescrita en el orden exacto del decompile (raw 004C4650, ~2800 lineas de
// las que ~60 % es hash-table anti-tamper).  La version anterior, armada con
// helpers y con ramas del 5.2 mezcladas, se saco en la S6.
//
// Contrato de la lista de texto (igual que en el binario):
//   TextList      = TextList (30 x 100)   TextNum
//   TextListColor, TextBold
//   SkipNum (dword_7EAA158) = DAT_07eaa158 (lineas de media altura)
// Unica desviacion: RII_Line devuelve un scratch si el indice se pasa de 30,
// para no escribir fuera del buffer (el original no tiene tope).
// ═════════════════════════════════════════════════════════════════════════════

void __cdecl ItemHelp_RequireClass(const BYTE* pItem);   // RequireClass (0x4C2880), CharMenu_Build.cpp

static char* RII_Line(int i)
{
    static char scratch[100];
    return (i >= 0 && i < 30) ? TextList + i * 100 : scratch;
}
static void RII_Style(int i, int color, int bold)
{
    if (i < 0 || i >= 30) return;
    TextListColor[i] = color;
    TextBold[i] = bold;
}
static void RII_Color(int i, int color) { if (i >= 0 && i < 30) TextListColor[i] = color; }
static void RII_Gold(char* buf, int v)       // mismo formato que el binario
{
    if      (v < 1000)       sprintf(buf, "%d", v % 1000);
    else if (v < 1000000)    sprintf(buf, "%d,%03d", v % 1000000 / 1000, v % 1000);
    else if (v < 1000000000) sprintf(buf, "%d,%03d,%03d", v % 1000000000 / 1000000,
                                     v % 1000000 / 1000, v % 1000);
    else                     sprintf(buf, "%d,%03d,%03d,%03d", v / 1000000000,
                                     v % 1000000000 / 1000000, v % 1000000 / 1000, v % 1000);
}
// Linea simple: texto + color + negrita, y avanza TextNum.
#define RII_ADD(color, bold, ...) \
    do { sprintf(RII_Line(TextNum), __VA_ARGS__); \
         RII_Style(TextNum, (color), (bold)); ++TextNum; } while (0)
// Texto sin argumentos: el binario lo pasa como FORMATO (sprintf(l, GlobalText[N]));
// CopyCollapsingPercent hace lo mismo (%% -> %) sin leer la pila si el texto trae %d.
#define RII_TXT(color, bold, idx) \
    do { CopyCollapsingPercent(RII_Line(TextNum), 100, GlobalText[idx]); \
         RII_Style(TextNum, (color), (bold)); ++TextNum; } while (0)
// Opcion con valor fijo (622..635): el Text.bmd del 0.97k traia el numero en el
// texto; el que usamos trae %d, asi que se pasa el valor que tiene la opcion
// (GetInventorySpecialOptionText, opcion = texto - 556).  Desviacion forzada.
#define RII_OPT(color, bold, idx) \
    do { GetInventorySpecialOptionText(type, (BYTE)((idx) - 556), 0, 0, RII_Line(TextNum), 100); \
         RII_Style(TextNum, (color), (bold)); ++TextNum; } while (0)
// Separador de media altura: "\n" + SkipNum.
#define RII_GAP() do { sprintf(RII_Line(TextNum), "\n"); ++TextNum; ++DAT_07eaa158; } while (0)

// SommonTable (0x00559FE4) y ChaosEventName (0x00559FFC, 10 x 100 bytes).
static const int kRII_SommonTable[6] = { 2, 7, 14, 8, 9, 41 };
static const char* const kRII_ChaosEventName[10] = {   // bytes EUC-KR leidos de 0x00559FFC
    "\310\367\265\371\305\251\040\260\355\307\342\040\277\251\307\340\261\307",
    "\306\346\306\274\276\366\064\040\304\304\307\273\305\315",
    "\265\360\301\366\305\273\304\253\270\336\266\363",
    "\267\316\301\366\305\330\040\271\253\274\261\040\270\266\277\354\275\272\053\305\260\272\270\265\345\040\274\274\306\256",
    "\062\065\066\115\040\267\245",
    "\066\260\263\277\371\040\300\342\301\366\040\261\270\265\266\261\307",
    "\271\256\310\255\273\363\307\260\261\307\050\270\270\277\370\051",
    "\271\302\040\270\323\261\327\304\305",
    "\271\302\040\124\274\305\303\367",
    "\271\302\040\061\060\275\303\260\243\040\271\253\267\341\300\314\277\353\261\307"
};

static void RII_RequireLine(int textIdx, int required, int have)
{
    // IDA: linea del requisito en color 0 si se cumple; si no, color 2 y una
    // linea extra GlobalText[74] con lo que falta, tambien en color 2.
    RII_ADD(0, 0, GlobalText[textIdx], required);
    if (have < required) {
        RII_Color(TextNum - 1, 2);
        RII_ADD(2, 0, GlobalText[74], required - have);
    }
}

// Nombre del item tal como lo arma el tooltip (IDA RenderItemInfo L700-830).
// `color` sólo se toca en las ramas que lo fijan.
static void RII_FormatName(const ITEM* ip, const ITEM_ATTRIBUTE* p, short type, int Level,
                           bool exc, char* name, int& color)
{
    if (type >= 471 && type <= 474) {
        sprintf(name, "%s", p->Name);
        color = 3;
    } else switch (type) {
    case 460:
        if      (Level == 0) sprintf(name, "%s", GlobalText[100]);
        else if (Level == 1) sprintf(name, "%s", GlobalText[101]);
        else if (Level == 2 && ip->Durability < 10) sprintf(name, "%s", kRII_ChaosEventName[ip->Durability]);
        break;
    case 459:
        switch (Level) {
        case 0: sprintf(name, "%s", p->Name); break;
        case 1: sprintf(name, "%s", GlobalText[105]); break;
        case 2: sprintf(name, "%s", GlobalText[106]); break;
        case 3: sprintf(name, "%s", GlobalText[107]); break;
        case 5: sprintf(name, "%s", GlobalText[109]); break;
        case 6: sprintf(name, "%s", GlobalText[110]); break;
        case 7: sprintf(name, "%s", GlobalText[111]); break;
        case 8: case 9: case 10: case 11: case 12:
            sprintf(name, "%s +%d", GlobalText[115], Level - 7); break;
        default: break;
        }
        break;
    case 431: {
        color = 3;
        static const int kText[4] = { 168, 169, 167, 166 };
        if (Level < 4) sprintf(name, "%s %s", GlobalText[kText[Level]], p->Name);
        break;
    }
    case 430:
        color = 3;
        sprintf(name, "%s", p->Name);
        break;
    case 469:
        color = 3;
        if      (Level == 0) sprintf(name, "%s", p->Name);
        else if (Level == 1) sprintf(name, "%s", GlobalText[810]);
        break;
    case 435:
        color = 3;
        if      (Level == 0) sprintf(name, "%s", GlobalText[811]);
        else if (Level == 1) sprintf(name, "%s", GlobalText[812]);
        else if (Level == 2) sprintf(name, "%s", GlobalText[817]);
        break;
    case 457:
        if      (Level == 0) sprintf(name, "%s", p->Name);
        else if (Level == 1) sprintf(name, "%s", GlobalText[108]);
        break;
    case 395:
        // Entrada 30 + Level de SkillAttribute (40 bytes, nombre en +0).
        sprintf(name, "%s %s", (const char*)&SkillAttribute + 8 * (5 * Level + 150), GlobalText[102]);
        break;
    case 426:
        if (Level < 6) {       // el original lee SommonTable sin tope
            const MONSTER_SCRIPT* monsters = (const MONSTER_SCRIPT*)&MonsterScript;
            for (int i = 0; i < MAX_MONSTER; ++i) {
                if (monsters[i].Type == kRII_SommonTable[Level]) {
                    sprintf(name, "%s %s", monsters[i].Name, GlobalText[103]);
                    break;
                }
            }
        }
        break;
    default:
        if ((type < 387 || type > 390) && type != 19 && type != 146 && type != 170 && exc) {
            if (Level) sprintf(name, "%s %s +%d", GlobalText[620], p->Name, Level);
            else       sprintf(name, "%s %s", GlobalText[620], p->Name);
        } else {
            if (Level) sprintf(name, "%s +%d", p->Name, Level);
            else       sprintf(name, "%s", p->Name);
        }
        break;
    }
}

// 0.97.20: el mismo nombre para el aviso de "levantaste X" (DLL
// CProtocol::CGItemGetRecv usa su propio GetItemName con las mismas reglas).
void Item_FormatName(const ITEM* ip, char* name)
{
    name[0] = 0;
    const std::uintptr_t attrBase = ItemAttribute_Base();
    if (!attrBase || !ip || ip->Type < 0 || ip->Type >= ITEM_MAX_EX) return;
    const ITEM_ATTRIBUTE* p = &((const ITEM_ATTRIBUTE*)(uintptr_t)attrBase)[ip->Type];
    int color = 0;
    RII_FormatName(ip, p, (short)ItemBehaviorType(ip->Type), (ip->Level >> 3) & 0xF,
                   (ip->Option1 & 0x3F) != 0, name, color);
}

// IDA: RenderItemInfo (0x004C4650)
static void RenderItemInfo_IDA(int sx, int sy, ITEM* ip, bool Sell)
{
    const std::uintptr_t attrBase = ItemAttribute_Base();
    if (!attrBase || !ip || (uintptr_t)ip < 0x100000) return;
    if (ip->Type < 0 || ip->Type >= ITEM_MAX_EX) return;
    ITEM_ATTRIBUTE* p = &((ITEM_ATTRIBUTE*)(uintptr_t)attrBase)[ip->Type];
    // 0.97.20: las líneas que el tooltip decide por rango de tipo (durabilidad,
    // una/dos manos, skill, opciones) usan el vanilla que imita un agregado;
    // nombre y valores salen de la fila del propio item (`p` / `ip`).
    const short type = (short)ItemBehaviorType(ip->Type);
    const BYTE* CA = (const BYTE*)CharacterAttribute;
    if (!CA) return;

    TextNum = 0;
    DAT_07eaa158 = 0;
    for (int i = 0; i < 20; ++i) TextListColor[i] = 0;    // memset(TextListColor, 0, 0x50)
    memset(TextList, 0, 30 * 100);             // TextList .. &pPickedItem
    RII_GAP();

    const int Level = (ip->Level >> 3) & 0xF;
    const bool exc = (ip->Option1 & 0x3F) != 0;

    // ── Color del nombre (v316) ─────────────────────────────────────────────
    int color;
    switch (type) {
    case 461: case 462: case 399: case 464: case 470:
    case 465: case 466: case 467: case 432: case 433:
        color = 3; break;
    case 170: case 19: case 146:
        color = 6; break;
    default:
        if (ip->SpecialNum && exc) color = 4;
        else                       color = (Level < 7) ? (ip->SpecialNum != 0) : 3;
        break;
    }
    if (type >= 387 && type <= 390)
        color = (Level < 7) ? (ip->SpecialNum != 0) : 3;

    // ── Precio (solo con la tienda abierta) ─────────────────────────────────
    if (type != 460 && (type < 471 || type > 474) && ShopOpened) {
        char buf[32];
        RII_Gold(buf, Item_CalculateValue((void*)ip, Sell ? 0 : 1));
        RII_ADD(color, 1, GlobalText[Sell ? 62 : 63], buf);
        RII_GAP();
    }

    // ── Nombre ──────────────────────────────────────────────────────────────
    const int nameIdx = TextNum;
    char* name = RII_Line(nameIdx);
    RII_FormatName(ip, p, type, Level, exc, name, color);
    RII_Style(nameIdx, color, 1);
    TextNum = nameIdx + 1;
    RII_GAP();

    // ── Tipo 435 (L840-1015) ────────────────────────────────────────────────
    if (type == 435) {
        RII_TXT(0, 0, 730);
        RII_TXT(5, 0, 815);
        // "\n" sin SkipNum: IDA sólo pone TextBold = 0 y avanza.
        sprintf(RII_Line(TextNum), "\n"); if (TextNum < 30) TextBold[TextNum] = 0; ++TextNum;
        int atkSpeed = 0, reqStr = 0, reqDex = 0;          // v79 / v78 / v80
        if (Level == 0)      { RII_ADD(0, 0, "%s: %d ~ %d", GlobalText[42], 107, 110); atkSpeed = 20; reqStr = 132; reqDex = 32; }
        else if (Level == 1) { RII_ADD(0, 0, "%s: %d ~ %d", GlobalText[40], 110, 120); atkSpeed = 35; reqStr = 381; reqDex = 149; }
        else if (Level == 2) { RII_ADD(0, 0, "%s: %d ~ %d", GlobalText[41], 120, 140); atkSpeed = 35; reqStr = 140; reqDex = 350; }
        // Level >= 3: el original usa un local sin inicializar (v323); queda en 0.
        RII_ADD(0, 0, GlobalText[64], atkSpeed);
        RII_ADD(0, 0, GlobalText[73], reqStr);
        RII_ADD(0, 0, GlobalText[75], reqDex);
        sprintf(RII_Line(TextNum), "\n"); if (TextNum < 30) TextBold[TextNum] = 0; ++TextNum;
        RII_TXT(1, 0, 87);
        RII_ADD(1, 0, GlobalText[94], 20);
        int mana = 0;
        if (Level == 0) {
            RII_ADD(1, 1, GlobalText[79], 53);
            RII_OPT(1, 0, 631);
            RII_OPT(1, 0, 632);
        } else if (Level == 1) {
            GetSkillInformation(22, 1, 0, &mana, 0, 0);
            RII_ADD(1, 0, GlobalText[84], mana);
            RII_OPT(1, 0, 629);
            RII_OPT(1, 0, 630);
        } else if (Level == 2) {
            GetSkillInformation(24, 1, 0, &mana, 0, 0);
            RII_ADD(1, 0, GlobalText[86], mana);
            RII_OPT(1, 0, 629);
            RII_OPT(1, 0, 630);
        }
        RII_OPT(1, 0, 628);               // LABEL_214
        RII_ADD(1, 0, GlobalText[633], 7);
        RII_OPT(1, 0, 634);
        RII_OPT(1, 0, 635);
    }

    // ── Huevos 471-474 / 460 / 467 (L1016-1070) ─────────────────────────────
    if (type >= 471 && type <= 474) {
        RII_TXT(0, 0, 730);
        RII_TXT(2, 0, 731);
        RII_TXT(2, 0, 732);
        RII_TXT(2, 0, 733);
    } else if (type == 460 && Level <= 1) {
        RII_TXT(0, 0, 119);
    }
    if (type == 467) {
        RII_TXT(0, 0, 638);
        RII_TXT(0, 0, 639);
    }

    // ── Daño / defensas / velocidades ───────────────────────────────────────
    if (ip->DamageMin) {
        if ((type & 0xFFF0) == 240) {
            RII_ADD(exc, 0, "%s: %d ~ %d", GlobalText[42], ip->DamageMin, ip->DamageMax);
        } else if (type == 485 || type == 494 || type == 495) {
            // El original no escribe linea: recolorea la ANTERIOR (v113 = v69 - 1).
            RII_Style(TextNum - 1, exc, 0);
        } else {
            int dmin = ip->DamageMin, dmax = ip->DamageMax;
            if (type >= 160 && type <= 192) { dmin /= 2; dmax /= 2; }
            RII_ADD(exc, 0, "%s: %d ~ %d", GlobalText[p->TwoHand + 40], dmin, dmax);
        }
    }
    if (ip->Defense)
        RII_ADD((type >= 224 && type < 384 && exc) ? 1 : 0, 0, GlobalText[65], ip->Defense);
    if (ip->MagicDefense)
        RII_ADD(0, 0, GlobalText[66], ip->MagicDefense);
    if (p->DefenseRate)
        RII_ADD(exc, 0, GlobalText[67], ip->SuccessfulBlocking);
    if (p->AttackSpeed)
        RII_ADD(0, 0, GlobalText[64], p->AttackSpeed);
    if (p->WalkSpeed)
        RII_ADD(0, 0, GlobalText[68], p->WalkSpeed);

    // ── Lineas por tipo (L1150-1250) ────────────────────────────────────────
    if (type == 459) {
        if (Level == 7) {
            RII_TXT(0, 0, 112);
            RII_TXT(0, 0, 113);
            RII_TXT(0, 0, 114);
        } else {
            RII_TXT(0, 0, 571);
        }
    }
    if (type == 461)                RII_TXT(0, 0, 572);
    if (type == 462)                RII_TXT(0, 0, 573);
    if (type == 464)                RII_TXT(0, 0, 621);
    if (type == 465 || type == 466) RII_TXT(0, 0, 637);
    if (type == 399)                RII_TXT(0, 0, 574);
    if (type == 470)                RII_TXT(0, 0, 619);
    if (type == 416) {
        RII_ADD(0, 0, GlobalText[578], 20);
        RII_ADD(0, 0, GlobalText[739], 50);
    }
    if (type == 417)                RII_TXT(0, 0, 576);

    // DESVIACION (DLL CustomWing, catálogo 0.97.20): aumento y reducción de
    // daño del ala custom con sus constantes.
    if (const CatalogWing* wing = gContentCatalog.GetItemWing(ip->Type)) {
        RII_ADD(0, 0, GlobalText[577], wing->IncDamageConstA + Level * wing->IncDamageConstB);
        RII_ADD(0, 0, GlobalText[578], wing->DecDamageConstA + Level * wing->DecDamageConstB);
        RII_TXT(0, 0, 579);
    } else if (type >= 384 && type <= 386) {               // alas de primera
        RII_ADD(0, 0, GlobalText[577], 2 * Level + 12);
        RII_ADD(0, 0, GlobalText[578], 2 * Level + 12);
        RII_TXT(0, 0, 579);
    } else if (type >= 387 && type <= 390) {               // alas de segunda
        RII_ADD(0, 0, GlobalText[577], Level + 32);
        RII_ADD(0, 0, GlobalText[578], Level + 25);
        RII_TXT(0, 0, 579);
    } else {
        switch (type) {
        case 419:
            RII_ADD(0, 0, GlobalText[577], 15);
            RII_ADD(0, 0, GlobalText[578], 10);
            break;
        case 430:
            RII_Color(TextNum, 0);
            sprintf(RII_Line(TextNum), "%s", GlobalText[748]);
            ++TextNum;
            break;
        case 431: {
            RII_Color(TextNum, 5);
            static const int kText[4] = { 168, 169, 167, 166 };
            if (Level < 4)
                sprintf(RII_Line(TextNum), "%s %s", GlobalText[kText[Level]], GlobalText[636]);
            ++TextNum;                                // aun con Level >= 4 (linea vacia)
            break;
        }
        case 432: case 433:
            RII_Color(TextNum, 0);
            sprintf(RII_Line(TextNum), "%s", GlobalText[816]);
            ++TextNum;
            break;
        case 434:
            RII_Color(TextNum, 0);
            sprintf(RII_Line(TextNum), "%s", GlobalText[814]);
            ++TextNum;
            sprintf(RII_Line(TextNum), "\n");
            ++TextNum;
            RII_TXT(0, 0, 638);
            RII_TXT(0, 0, 639);
            break;
        case 469:
            RII_Color(TextNum, 0);
            ++TextNum;                                // linea vacia, como el original
            break;
        default:
            break;
        }
    }

    // ── DESVIACION (0.97.20): líneas propias del item ("tooltip" en
    // Data/Custom/Items del server), debajo del nombre.
    for (int i = 0; i < gContentCatalog.GetTooltipCount(ip->Type); ++i) {
        const Proto::CATALOG_TOOLTIP* line = gContentCatalog.GetTooltipLine(ip->Type, i);
        if (line) RII_ADD(line->Color, 0, "%s", line->Text);
    }

    // ── Durabilidad (LABEL_307..331) ────────────────────────────────────────
    {
        const bool hasDur =
            ((p->Durability || p->MagicDurability) && (type < 384 || type >= 416) && type < 448) ||
            (type >= 384 && type <= 390);
        if (!hasDur && !ip->Durability) {
            if (type == 426) RII_ADD(0, 0, GlobalText[95], ip->Durability);
        } else if (type >= 430 && type <= 435) {
            // LABEL_318: sin linea salvo el 426 (que no cae en este rango)
        } else if ((type >= 448 && type <= 456) || type == 135 || type == 143 ||
                   // DESVIACION (DLL ItemStack): los apilables del server muestran la cantidad.
                   ItemStack_GetMaxStack(type, Level) != 0) {
            RII_ADD(0, 0, GlobalText[69], ip->Durability);
        } else if (type >= 416 && type <= 423) {
            RII_ADD(0, 0, GlobalText[70], ip->Durability);
        } else if (type == 426) {
            RII_ADD(0, 0, GlobalText[95], ip->Durability);
        } else if (hasDur) {
            const unsigned int maxDur =
                Item_CalculateMaxDurability((void*)ip, p, Level) & 0xFFFF;
            RII_ADD(0, 0, GlobalText[71], ip->Durability, (int)maxDur);
        }
    }
    if ((type == 135 || type == 143) && Level >= 1) {       // flechas / bolts
        RII_ADD(1, 0, GlobalText[577], 2 * Level + 1);
        RII_ADD(1, 0, GlobalText[88], 1);
    }

    // ── Resistencias (GlobalText[48..51]) ───────────────────────────────────
    for (int r = 0; r < 4; ++r)
        if (p->Resistance[r])
            RII_ADD(0, 0, GlobalText[72], GlobalText[48 + r], Level + 1);

    // ── Requisitos ──────────────────────────────────────────────────────────
    if (ip->RequireStrength)
        RII_RequireLine(73, ip->RequireStrength, *(const WORD*)(CA + 20));
    if (ip->RequireDexterity)
        RII_RequireLine(75, ip->RequireDexterity, *(const WORD*)(CA + 22));
    if (ip->RequireLevel && type != 430)
        RII_RequireLine(76, ip->RequireLevel, *(const WORD*)(CA + 14));
    // Tipo 395: el original compara contra un local (v322) que el decompile no
    // deja ver; se usa RequireEnergy, que es lo que imprime la linea.
    if (type == 395 || ip->RequireEnergy)
        RII_RequireLine(77, ip->RequireEnergy, *(const WORD*)(CA + 26));

    if (type != 435)
        ItemHelp_RequireClass(reinterpret_cast<const BYTE*>(p));

    // ── Lineas de botas / guantes / bastones ────────────────────────────────
    if (type >= 352 && type < 384 && Level >= 5) {
        RII_GAP();
        RII_TXT(1, 1, 78);
    }
    if (type >= 320 && type < 352 && Level >= 5) {
        RII_GAP();
        RII_TXT(1, 1, 93);
    }
    if ((type >= 160 && type < 192) || type == 31) {
        RII_GAP();
        RII_ADD(1, 1, GlobalText[79], (ip->DamageMin >> 1) + 2 * Level);
    }

    // ── Opciones especiales (L2045-2200) ────────────────────────────────────
    if (ip->SpecialNum) RII_GAP();
    for (int i = 0; i < ip->SpecialNum && i < 9; ++i) {
        const int s   = ip->Special[i];
        const int val = ip->SpecialValue[i];
        // Texto de la opcion: mismo switch que IDA (L2064-2175), via el helper
        // que ya pasa los valores fijos de 66..79 y colapsa el %% de [87].  Para
        // opciones sin texto (default, y 65 en 430..434) deja la linea vacia, que
        // es lo que hace el original al saltar a LABEL_660.
        int mana = 0;
        if ((s >= 18 && s <= 24) || s == 49 || s == 56)
            GetSkillInformation(s, 1, 0, &mana, 0, 0);
        GetInventorySpecialOptionText(type, (BYTE)s, (BYTE)val, mana, RII_Line(TextNum), 100);
        RII_Style(TextNum, 1, 0);
        ++TextNum;
        if (s == 64)                     RII_ADD(1, 0, GlobalText[94], val);
        else if (s == 49)                RII_TXT(5, 0, 179);
        else if (type == 31 && s == 60)  RII_ADD(1, 0, GlobalText[89], val);
    }
    RII_GAP();

    // ── Epilogo: alto del recuadro y dibujo (identico al port anterior) ─────
    SIZE sz = { 0, 0 };
    gFont.MeasureText(gFont.GetTextDC(), TextList, 1, &sz);
    const int h = sz.cy * DAT_07eaa158 / 2 + sz.cy * (TextNum - DAT_07eaa158);
    int y = sy - (int)((float)h / gWindow.GetScreenRateY());
    if (y < 0) y = sy + 20 * p->Height;
    CharMenu_RenderTextList(sx, y, TextNum < 30 ? TextNum : 30, 0, 2, 1);
}
#undef RII_ADD
#undef RII_TXT
#undef RII_OPT
#undef RII_GAP

// Pools que pueden entregar el item seleccionado (Item_ClickHandler y
// SecondPassword). No leer Type ni opciones hasta comprobar slot y alineacion.
extern "C" BYTE ShopItems[120 * 68];
static bool IsTooltipItem(const void* candidate)
{
    if (!candidate) return false;
    const uintptr_t address = (uintptr_t)candidate;
    const auto inSlots = [address](const void* base, size_t count) {
        const uintptr_t start = (uintptr_t)base;
        return start && address >= start && address - start < count * sizeof(ITEM) &&
               (address - start) % sizeof(ITEM) == 0;
    };
    const bool known = candidate == pPickedItem ||
        inSlots(OffsetInventoryItems, 64) || inSlots(OffsetTradeItems, 64) ||
        inSlots(OffsetMixItems, 64) || inSlots(OffsetWarehouseItems, 120) ||
        inSlots(Inventory, 160) || inSlots(ShopItems, 120) ||
        (CharacterMachine &&
         inSlots((const BYTE*)CharacterMachine + 536, 12));
    if (!known) return false;
    const ITEM* item = static_cast<const ITEM*>(candidate);
    const std::uintptr_t base = ItemAttribute_Base();
    if (!base || item->Type < 0 || item->Type >= ITEM_MAX_EX ||
        item->SpecialNum > MAX_SPECIAL_OPTION) return false;
    const ITEM_ATTRIBUTE& attr = ((const ITEM_ATTRIBUTE*)(uintptr_t)base)[item->Type];
    return memchr(attr.Name, 0, sizeof(attr.Name)) != nullptr;
}

void __cdecl RenderItemInfo(void* param_1, void* param_2, void* param_3_v, int param_4)
{
    if (!IsTooltipItem(param_3_v)) { CheckInventory = 0; return; }
    if (!Platform::RunValidatedOperation([&]() {
        RenderItemInfo_IDA((int)(uintptr_t)param_1, (int)(uintptr_t)param_2,
                           (ITEM*)param_3_v, param_4 != 0);
    })) {
        DbgLogPublic("RIP CRASHED inside _impl — caught by SEH");
        extern uintptr_t CheckInventory;
        CheckInventory = 0;
    }
}

// IDA: RenderRepairInfo (0x004C8D70)
void __cdecl RenderRepairInfo(void* param_1, int param_2, void* param_3_v)
{
    if (!IsTooltipItem(param_3_v)) { CheckInventory = 0; return; }
    if (!Platform::RunValidatedOperation([&]() {
        RenderRepairInfo_impl(param_1, param_2, param_3_v);
    })) {
        DbgLogPublic("RRI CRASHED inside _impl — caught by SEH");
        extern uintptr_t CheckInventory;
        CheckInventory = 0;
    }
}

// RenderRepairInfo @ 0x004C8D70 — RenderRepairInfo(param_1, param_2, ITEM* ip)    [Kayito: RenderRepairInfo]
// Shows item tooltip in the repair NPC context. unaff_EBX=DAT_07cf1ffc, unaff_ESI=1 (anti-tamper).
// 0x0055A63C: separador de media altura de RenderRepairInfo (lo agrego main, PR #35).
static const char DAT_0055a63c[] = "\n";

extern "C" void __cdecl RenderRepairInfo_impl(void* param_1, int param_2, void* param_3_v) // RenderRepairInfo
{
    // Guardas defensivas sobre el item y la tabla de atributos.
    // Use the backup-aware accessor to recover DAT_07d78068 if clobbered.
    std::uintptr_t attrBaseOK_ = ItemAttribute_Base();
    if (attrBaseOK_ == 0) return;
    if (param_3_v == nullptr || (uintptr_t)param_3_v < 0x100000) return;
    unsigned short* param_3 = (unsigned short*)param_3_v;
    short itemType = (short)*param_3;
    if (itemType < 0 || (unsigned short)itemType >= ITEM_MAX_EX) return;

    // Class-filter exclusions
    if (itemType > 0x19f && itemType < 0x1a4) return;
    if (itemType == 0x1aa) return;
    if (itemType == 0x87)  return;
    if (itemType == 0x8f)  return;
    if (itemType > 0x1bf)  return;
    if (itemType > 0x186 && itemType < 0x194) return;
    if (itemType > 0x1ad && itemType < 0x1b4) return;
    if (itemType == 0x1d5) return;
    if (IsRenderRepairInfoBlocked(itemType)) return;

    const std::uintptr_t attrBase = itemType * 0x40 + attrBaseOK_;

    TextNum = 0;
    DAT_07eaa158 = 0;

    for (int i = 0; i < 30; ++i)
        TextList[i * 100] = 0;

    unsigned int level = (*(unsigned int*)(param_3 + 2) >> 3) & 0xf;

    // Quality tier
    unsigned int tier = 0;
    if (itemType == 0x1cd || itemType == 0x1ce || itemType == 399 ||
        itemType == 0x1d1 || itemType == 0x1d2 || itemType == 0x1d3) {
        tier = 3;
    } else if (itemType == 0xaa || itemType == 0x13 || itemType == 0x92) {
        tier = 6;
    } else {
        char opt1      = (char)param_3[0x12];
        unsigned char optFlags = *(unsigned char*)((char*)param_3 + 0x1b) & 0x3f;
        if (opt1 != '\0' && optFlags != 0) {
            tier = 4;
        } else {
            tier = (level > 6) ? 3u : (opt1 != '\0' ? 1u : 0u);
        }
    }
    if (itemType > 0x182 && itemType < 0x187) {
        tier = (level < 7) ? (unsigned int)((char)param_3[0x12] != '\0') : 3;
    }

    // Slot 0: name
    crt_sprintf(TextList, DAT_0055a5f4);
    TextNum++;
    DAT_07eaa158++;

    // Slot 1: Costo de reparacion
    unsigned int maxDur = Item_CalculateMaxDurability(param_3, (ITEM_ATTRIBUTE*)attrBase, (int)level) & 0xffff;
    unsigned int curDur = (unsigned int)*(unsigned char*)((char*)param_3 + 0x1a);
    // IDA RenderRepairInfo: RepairEnable_0 = 1 con el item sano y = 2 con el
    // item dañado; el 2 es el martillo animado de RenderCursor, y Scene_MapTick
    // lo vuelve a 1 cada frame.
    // IDA L133-150: la linea del costo es sprintf(GlobalText[238], Buffer), con
    // Buffer = ConvertRepairGold(...) si el item esta danado y "0" (0x55A5F8)
    // si esta sano; color = tier (v12), en negrita.
    char repairGold[64] = "0";
    if (curDur < maxDur) {
        RepairEnable_0 = 2;
        // Same ItemValue/ConvertRepairGold pair.
        int gold = Item_CalculateValue((void*)param_3, 2);
        Item_CalculateRepairCost(gold, (int)curDur, (int)maxDur, (short)itemType, repairGold);
    } else {
        RepairEnable_0 = 1;
    }
    crt_sprintf(TextList + TextNum * 100, GlobalText[238], repairGold);
    TextListColor[TextNum] = (int)tier;   // TextListColor = v12
    TextBold[TextNum] = 1;
    TextNum++;

    // Slot 2: separador
    crt_sprintf(TextList + TextNum * 100, DAT_0055a5fc);
    TextNum++;
    DAT_07eaa158++;

    // Slot 3: nombre del item
    {
        char* dst = TextList + TextNum * 100;
        const char* itemName = ((ITEM_ATTRIBUTE*)(uintptr_t)attrBase)->Name;
        unsigned char optFlags = *(unsigned char*)((char*)param_3 + 0x1b) & 0x3f;

        if (itemType >= 0x183 && itemType <= 0x186) {
            if (level == 0) {
                crt_sprintf(dst, "%s", itemName);
            } else {
                crt_sprintf(dst, "%s +%d", itemName, level);
            }
        } else {
            if (optFlags == 0) {
                if (level == 0) {
                    crt_sprintf(dst, "%s", itemName);
                } else {
                    crt_sprintf(dst, "%s +%d", itemName, level);
                }
            } else {
                if (level == 0) {
                    crt_sprintf(dst, "%s %s", GlobalText[620], itemName);
                } else {
                    crt_sprintf(dst, "%s %s +%d", GlobalText[620], itemName, level);
                }
            }
        }
    }
    TextBold[TextNum] = 1;
    TextListColor[TextNum] = tier;
    TextNum++;

    // Slot 4: separador
    crt_sprintf(TextList + TextNum * 100, DAT_0055a63c);
    TextNum++;
    DAT_07eaa158++;

    // Slot 5: durabilidad (solo si itemType < 0x1c0)
    if (itemType < 0x1c0) {
        crt_sprintf(TextList + TextNum * 100, GlobalText[71], (int)curDur, (int)maxDur);
        TextListColor[TextNum] = 0;
        TextBold[TextNum] = 0;
        TextNum++;
    }

    // Epilogo fiel a 0x004c9664..0x004c971b.  Igual que RenderItemInfo salvo
    // que la conversion vertical es entera: (h * 15 * 32) / WindowHeight, que
    // es el equivalente de h / g_fScreenRate_y con DIV sin signo.
    {
        SIZE sz;
        int  h;
        int  yBox;

        crt_sprintf(TextList + TextNum * 100, DAT_0055a640);
        TextNum++;
        DAT_07eaa158++;

        sz.cx = 0;
        sz.cy = 0;
        gFont.MeasureText(gFont.GetTextDC(), TextList, 1, &sz);
        h = (TextNum - DAT_07eaa158) * sz.cy + (DAT_07eaa158 * sz.cy) / 2;
        yBox = param_2 - (int)((unsigned int)(h * 15 * 32) / gWindow.GetHeight());
        if (yBox < 0) {
            yBox = param_2 +
                   (int)((ITEM_ATTRIBUTE*)(uintptr_t)attrBaseOK_)[itemType].Height * 0x14;
        }
        CharMenu_RenderTextList((int)(uintptr_t)param_1, yBox, TextNum, 0, 2, 1);
    }
}
