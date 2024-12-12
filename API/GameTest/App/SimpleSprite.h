//-----------------------------------------------------------------------------
// SimpleSprite.h
// Provides a simple way to display still images or animated images (via sprite sheets).
//-----------------------------------------------------------------------------
#ifndef _SIMPLESPRITE_H_
#define _SIMPLESPRITE_H_

#include "../glut/include/GL/freeglut.h"
#include <map>
#include <vector>
#include <string>

//-----------------------------------------------------------------------------
// CSimpleSprite
//-----------------------------------------------------------------------------
class CSimpleSprite
{
public:
    // If width, height and UV coords are not provided then they will be derived from the texture size.
    CSimpleSprite(const char *fileName, const unsigned int nColumns = 1, const unsigned int nRows = 1);
    void Update(const float dt);
    void Draw();
    void SetPosition(const float x, const float y) { m_xpos = x; m_ypos = y; }   
    void SetAngle(const float a)  { m_angle = a; }
    void SetScale(const float s) { m_scale = s >= 0.0f ? s : 0.0f; }
    void SetFrame(const unsigned int f);
    void SetAnimation(const int id);
    void SetAnimation(const int id, const bool playFromBeginning);
	void GetPosition(float &x, float &y) { x = m_xpos; y = m_ypos; }
    float GetWidth()  const { return m_width;  }
    float GetHeight() const { return m_height; }
    float GetAngle()  const { return m_angle;  }
    float GetScale()  const { return m_scale;  }
    unsigned int GetFrame()  const { return m_frame; }
	void SetColor(const float r, const float g, const float b) { m_red = r; m_green = g; m_blue = b; }

    // Note: speed must be > 0, frames must have size >= 1, id must be unique among animations
    void CreateAnimation(const unsigned int id, const float speed, const std::vector<int> &frames)
    {
        sAnimation anim;        
        anim.m_id = id;
        anim.m_speed = speed;
        anim.m_frames = frames;
        m_animations.push_back(anim);        
    };

private:
    void CalculateUVs();
    GLuint m_texture;
    float m_xpos = 0.0f;
    float m_ypos = 0.0f;
    float m_width = 0.0f;
    float m_height = 0.0f;
    int   m_texWidth = 0;
    int   m_texHeight = 0;
    float m_angle = 0.0f;
    float m_scale = 1.0f;
    float m_points[8];    
    float m_uvcoords[8];
    unsigned int m_frame;
    unsigned int m_nColumns;
    unsigned int m_nRows;
	float m_red = 1.0f;
	float m_green = 1.0f;
	float m_blue = 1.0f;
    int   m_currentAnim = -1;
    float m_animTime = 0.0f;

    struct sAnimation
    {
        unsigned int m_id = 0;
        float m_speed = 0.0f;
        std::vector<int> m_frames;
    };
    std::vector<sAnimation> m_animations;

    // Texture management.
    struct sTextureDef
    {
        unsigned int m_width;
        unsigned int m_height;
        GLuint m_textureID;
    };
    bool LoadTexture(const std::string& filename);
    static std::map<std::string, sTextureDef> m_textures;    
};

#endif