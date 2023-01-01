/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
// Authors: Yosha Vandaele
/*=============================================================================*/
// EGraphNodeTypes.h: Various node types for graphs
/*=============================================================================*/

#pragma once

#include "EGraphEnums.h"
#include "EliteGraphUtilities/EGraphVisuals.h"

namespace Elite
{
	class GraphNode
	{
	public:
		GraphNode() : m_Index(invalid_node_index) {}
		explicit GraphNode(int idx) : m_Index(idx) {}

		virtual ~GraphNode() = default;

		int GetIndex() const { return m_Index; }
		void SetIndex(int newIdx) { m_Index = newIdx; }

		bool operator==(const GraphNode& rhs) { return m_Index == rhs.m_Index; }
		
	protected:
		int m_Index;
	};

	class GraphNode2D : public GraphNode
	{
	public:
		GraphNode2D(int index, const Elite::Vector2& pos = Elite::ZeroVector2)
			: GraphNode(index), m_Position(pos), m_Color(DEFAULT_NODE_COLOR)
		{
		}
		virtual ~GraphNode2D() = default;

		Elite::Vector2 GetPosition() const { return m_Position; }
		void SetPosition(const Elite::Vector2& newPos) { m_Position = newPos; }

		Elite::Color GetColor() const { return m_Color; }
		void SetColor(const Elite::Color& color) { m_Color = color; }

	protected:
		Elite::Vector2 m_Position;
		Elite::Color m_Color;
	};


	class GridTerrainNode : public GraphNode
	{
	public:
		GridTerrainNode(int index)
			: GraphNode(index), m_Terrain(TerrainType::Ground)
		{
		}
		virtual ~GridTerrainNode() = default;


		TerrainType GetTerrainType() const { return m_Terrain; }
		void SetTerrainType(TerrainType terrain) { m_Terrain = terrain; }
		Elite::Color GetColor() const 
		{
			switch (m_Terrain)
			{
			case TerrainType::Mud:
				return MUD_NODE_COLOR;
				break;
			case TerrainType::Water:
				return WATER_NODE_COLOR;
				break;
			default:
				return GROUND_NODE_COLOR;
				break;
			}
		}
		

	protected:
		TerrainType m_Terrain;
	};


	class NavGraphNode : public GraphNode2D
	{
	public:
		NavGraphNode(int index, const Vector2& pos = ZeroVector2)
			: GraphNode2D(index, pos), m_LineIdx(0)	{}
		NavGraphNode(int index, int lineIdx, const Vector2& pos = ZeroVector2)
			: GraphNode2D(index, pos), m_LineIdx(lineIdx){}
		virtual ~NavGraphNode() = default;
		int GetLineIndex() const { return m_LineIdx; };
	protected:
		int m_LineIdx;
	};

	class InfluenceNode final : public Elite::GraphNode2D
	{
	public:
		InfluenceNode(int index, Elite::Vector2 pos = { 0,0 }, float influence = 0.f)
			: GraphNode2D(index, pos), m_Influence(influence) {}

		float GetInfluence() const { return m_Influence; }
		void SetInfluence(float influence) { m_Influence = influence; }

	private:
		float m_Influence;
	};


	// TODO: not my code, look more into this 
	class FlowFieldNode : public GraphNode2D
	{
	public:
		FlowFieldNode(int idx)
			:GraphNode2D(idx), m_Direction{}
		{
			SetCost(1);
		}
		virtual ~FlowFieldNode() = default;

		Vector2 GetDirection() const { return m_Direction; }
		void SetDirection(Vector2 direction) { m_Direction = direction; }

		BYTE GetCost() const { return m_Cost; }
		void SetCost(BYTE cost)
		{
			m_Cost = cost;
			m_Color = Color{ 0.f,1.f - float(cost) / 255.f, 0.f };
		}

		size_t GetIntegrationValue() const { return m_IntegrationValue; }
		void SetIntegrationValue(size_t value) { m_IntegrationValue = value; }

		Elite::Color GetColor() const
		{
			if (m_ShowCost) return m_Color;
			else if (m_ShowIntegration) return Color{ 0.f,float(m_IntegrationValue) / 8.f, 0.f };
			else return DEFAULT_NODE_COLOR;
		}

		void SetShowCost(bool showCost) { m_ShowCost = showCost; }
		void SetShowIntegration(bool showIntegration) { m_ShowIntegration = showIntegration; }
	protected:
		Elite::Vector2 m_Direction{};
		BYTE m_Cost{ 1 };
		size_t m_IntegrationValue{};
		bool m_ShowCost{};
		bool m_ShowIntegration{};
	};
}