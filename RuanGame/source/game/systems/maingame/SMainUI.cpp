#include "stdafx.h"
#include "SMainUI.h"

void SMainUI::InitGUI() {
    m_measures = SEntityManager::Instance().AddEntity("ui");
    m_score_keeper = SEntityManager::Instance().AddEntity("ui");
    m_move_keeper = SEntityManager::Instance().AddEntity("ui");
}

void SMainUI::InstantiateGUI() const {
    CUIText ctext = {};
    ctext.pos = Vector3(400.0f, 100.0f, 0.0f);
    ctext.rgb = m_green;
    ctext.text = "[SPACE]: ";
    SEntityManager::Instance().AddComponent<CUIText>(m_measures, ctext);
    
    CUIText cscore = {};
    cscore.pos = Vector3(20.0f, SCREEN_HEIGHT - 20.0f, 0.0f);
    cscore.rgb = m_green;
    cscore.text = "STARS: [ ] [ ] [ ] [ ]";
    SEntityManager::Instance().AddComponent<CUIText>(m_score_keeper, cscore);
    
    CUIText cmoves = {};
    cmoves.pos = Vector3(20.0f, SCREEN_HEIGHT - 40.0f, 0.0f);
    cmoves.rgb = m_yellow;
    cmoves.text = "MOVES: 0";
    SEntityManager::Instance().AddComponent<CUIText>(m_move_keeper, cmoves);
}


void SMainUI::Update(const float deltaTime) {
    if (!IncrementRate(m_rate_counter, m_rate)) return; // do nothing if it's not time yet

    // update the launch gage
    CUIText* ctext = SEntityManager::Instance().GetComponent<CUIText>(m_measures);

    if (m_decrementing) m_gage_level = abs((m_gage_level + m_gage - 1) % m_gage);
    else m_gage_level = (m_gage_level + 1) % m_gage;

    if (m_gage_level == 0) {
        m_decrementing = !m_decrementing;
        if (m_decrementing) m_gage_level = m_gage - 1;
    }


    ctext->text = "[SPACE]: ";
    for (int i = 0; i < m_gage_level; i++) {
        ctext->text.append("> ");
    }

    // update the number of moves
    CUIText* cmoves = SEntityManager::Instance().GetComponent<CUIText>(m_move_keeper);
    if (SLevelManager::Instance().GetCurrentMoves() > m_moves_score) {
        m_moves_score++;
        cmoves->text = "MOVES: " + std::to_string(m_moves_score);
    }
}

void SMainUI::Shutdown() const {
    SEntityManager::Instance().RemoveEntity(m_score_keeper);
    SEntityManager::Instance().RemoveEntity(m_move_keeper);
    SEntityManager::Instance().RemoveEntity(m_measures);
}

void SMainUI::OnNotify(Event event, std::vector<EntityID> entities) {
    if (event != STAR_COLLECTED) return;

    CUIText* cscore = SEntityManager::Instance().GetComponent<CUIText>(m_score_keeper);

    m_star_score++;

    cscore->text = "STARS: ";
    for (int i = 0; i < m_star_score; i++) {
        cscore->text.append("[O] ");
    }
    for (int i = m_star_score; i < STARS; i++) {
        cscore->text.append("[ ] ");
    }
}