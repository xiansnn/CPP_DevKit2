#include "canvas.h"

#define RED_MASK 0xF8
#define GREEN_MASK 0xFC
#define BLUE_MASK 0xF8
#define RGB_TO_565(R, G, B) (((R & RED_MASK) << 8) | ((G & GREEN_MASK) << 3) | (B & BLUE_MASK) >> 3)

std::map<ColorIndex, uint16_t> color565_palette{
    {ColorIndex::BLACK, RGB_TO_565(0, 0, 0)},
    {ColorIndex::WHITE, RGB_TO_565(255, 255, 255)},  
    {ColorIndex::RED, RGB_TO_565(255, 0, 0)},        
    {ColorIndex::LIME, RGB_TO_565(0, 255, 0)},       
    {ColorIndex::BLUE, RGB_TO_565(0, 0, 255)},       
    {ColorIndex::YELLOW, RGB_TO_565(255, 255, 0)},   
    {ColorIndex::CYAN, RGB_TO_565(0, 255, 255)},     
    {ColorIndex::MAGENTA, RGB_TO_565(255, 0, 255)},  
    {ColorIndex::SILVER, RGB_TO_565(192, 192, 192)}, 
    {ColorIndex::GRAY, RGB_TO_565(128, 128, 128)},   
    {ColorIndex::BURGUNDY, RGB_TO_565(128, 0, 0)},     
    {ColorIndex::MAROON, RGB_TO_565(165, 42, 42)},     
    {ColorIndex::OLIVE, RGB_TO_565(128, 128, 0)},    
    {ColorIndex::GREEN, RGB_TO_565(0, 128, 0)},      
    {ColorIndex::PURPLE, RGB_TO_565(128, 0, 128)},   
    {ColorIndex::TEAL, RGB_TO_565(0, 128, 128)},     
    {ColorIndex::NAVY, RGB_TO_565(0, 0, 128)},       
    {ColorIndex::ORANGE, RGB_TO_565(255, 165, 0)},   
    {ColorIndex::GOLD, RGB_TO_565(255, 215, 0)},     
    {ColorIndex::FOREST, RGB_TO_565(34, 139, 34)}};  