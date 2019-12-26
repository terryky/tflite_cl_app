__kernel void detectLine(__global  uchar4* result, __global  uchar4* origin, int2 size)
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    if (x >= size.x || y >= size.y)
        return;
    
    int xm = max (0, x - 1);
    int ym = max (0, y - 1);
    int xp = min (size.x - 1, x + 1);
    int yp = min (size.y - 1, y + 1);

    int index = (size.x * y + x);

    float4 val = (float4)6.828 * convert_float4(origin[index]);
    val -= convert_float4 (origin[(size.x * y + (xm))]);
    val -= convert_float4 (origin[(size.x * y + (xp))]);
    val -= convert_float4 (origin[(size.x * (ym) + x)]);
    val -= convert_float4 (origin[(size.x * (yp) + x)]);
    val -= (float4)0.707 * convert_float4 (origin[(size.x * (ym) + (xm))]);
    val -= (float4)0.707 * convert_float4 (origin[(size.x * (ym) + (xp))]);
    val -= (float4)0.707 * convert_float4 (origin[(size.x * (yp) + (xm))]);
    val -= (float4)0.707 * convert_float4 (origin[(size.x * (yp) + (xp))]);

    result[index] = convert_uchar4_sat(val);
    result[index].a = 255;
}
