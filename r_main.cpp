#include "r_main.h"

#include "m_fixed.h"

Renderer::Renderer(const WAD& wad)
    : m_f_start{ wad.W_GetNumForName("F_START") }
    , m_f_end{ wad.W_GetNumForName("F_END") }
    , m_s_start{ wad.W_GetNumForName("S_START") }
    , m_s_end{ wad.W_GetNumForName("S_END") }
    , m_p_start{ wad.W_GetNumForName("P_START") }
    , m_p_end{ wad.W_GetNumForName("P_END") }
    , m_skyflatnum{ wad.W_GetNumForName("F_SKY1") }
{
    if (!(m_f_start < m_f_end))
        throw std::runtime_error("Flats start after end !");

    if (!(m_s_start < m_s_end))
        throw std::runtime_error("Sprites start after end !");

    if (!(m_p_start < m_p_end))
        throw std::runtime_error("Patches start after end !");

    if (m_skyflatnum < m_f_start || m_f_end < m_f_start)
        throw std::runtime_error("Sky is not a flat !");

    R_InitTextures(wad);
}

void Renderer::R_ExecuteSetViewSize()
{
    setsizeneeded = false;

    if (m_setblocks == 11)
    {
        m_scaledviewwidth = SCREENWIDTH;
        m_viewheight = SCREENHEIGHT;
    }
    else
    {
        m_scaledviewwidth = m_setblocks * 32;
        m_viewheight = (m_setblocks * 168 / 10) & ~7;
    }

    m_detailshift = m_setdetail;
    m_viewwidth = m_scaledviewwidth >> m_detailshift;

    m_centery = m_viewheight / 2;
    m_centerx = m_viewwidth / 2;
    m_centerxfrac = fixed_t::from_num(m_centerx, 0u);
    m_centeryfrac = fixed_t::from_num(m_centery, 0u);
    m_projection = m_centerxfrac;
/*
    R_InitTextureMapping();

    // psprite scales
    pspritescale = FRACUNIT * viewwidth / SCREENWIDTH;
    pspriteiscale = FRACUNIT * SCREENWIDTH / viewwidth;

    // thing clipping
    for (i = 0; i < viewwidth; i++)
        screenheightarray[i] = viewheight;

    fixed_t	cosadj;
    fixed_t	dy;
    int		i;
    int		j;
    int		level;
    int		startmap;

    // planes
    for (i = 0; i < viewheight; i++)
    {
        dy = ((i - viewheight / 2) << FRACBITS) + FRACUNIT / 2;
        dy = abs(dy);
        yslope[i] = FixedDiv((viewwidth << detailshift) / 2 * FRACUNIT, dy);
    }

    for (i = 0; i < viewwidth; i++)
    {
        cosadj = abs(finecosine[xtoviewangle[i] >> ANGLETOFINESHIFT]);
        distscale[i] = FixedDiv(FRACUNIT, cosadj);
    }

    // Calculate the light levels to use
    //  for each level / scale combination.
    for (i = 0; i < LIGHTLEVELS; i++)
    {
        startmap = ((LIGHTLEVELS - 1 - i) * 2) * NUMCOLORMAPS / LIGHTLEVELS;
        for (j = 0; j < MAXLIGHTSCALE; j++)
        {
            level = startmap - j * SCREENWIDTH / (viewwidth << detailshift) / DISTMAP;

            if (level < 0)
                level = 0;

            if (level >= NUMCOLORMAPS)
                level = NUMCOLORMAPS - 1;

            scalelight[i][j] = colormaps + level * 256;
        }
    }
    */
}