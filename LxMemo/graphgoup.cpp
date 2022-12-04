#include "graphgoup.h"
#include "textgraph.h"
#include "graphicgraph.h"

GraphGroup::GraphGroup(GraphicCanvas* canvas)
	: Graph{ canvas }
{
}

void GraphGroup::AddGraph(Graph* graph)
{
	graphs_.append(graph);
}

bool GraphGroup::DeleteGraph(Graph* graph)
{
	if (graph) {
		graphs_.removeOne(graph);
	}

	return true;
}

bool GraphGroup::DeleteAll()
{
	for (auto it = graphs_.begin(); it != graphs_.end();) {
		it = graphs_.erase(it);
	}
	return true;
}

Graph* GraphGroup::At(const QPoint& pos)
{
	Graph* find_graph = nullptr;

	for (auto& g : graphs_) {
		if (GraphType::kGraphGroup == g->Type())
			find_graph = g->converTo<GraphGroup>()->At(pos);
		else if (g->Geometry().contains(pos))
			find_graph = g;
		if (find_graph)
			break;
	}

	return find_graph;
}

Graph* GraphGroup::SelectGraph()
{
	Graph* find_graph = nullptr;

	for (auto& g : graphs_) {
		if (GraphType::kGraphGroup == g->Type())
			find_graph = g->converTo<GraphGroup>()->SelectGraph();
		else if (g->Selected())
			find_graph = g;
		if (find_graph)
			break;
	}

	return find_graph;
}

void GraphGroup::MouseClick(const QPoint& pos)
{
	for (auto rb = graphs_.rbegin();
		rb != graphs_.rend();
		++rb)
	{
		(*rb)->MouseClick(pos);
		if ((*rb)->Selected())
			break;
	}
}

void GraphGroup::MouseDoubleClick(const QPoint& pos)
{
	for (auto& g : graphs_)
		g->MouseDoubleClick(pos);
}

void GraphGroup::SerializeTo(QDataStream& stream)
{
	Graph::SerializeTo(stream);
	stream << graphs_.size();

	for (auto& g : graphs_)
		g->SerializeTo(stream);
}

void GraphGroup::DeserializeFrom(QDataStream& stream)
{
	Graph::DeserializeFrom(stream);
	decltype(graphs_.size()) count = 0;
	stream >> count;

	for (int i = 0; i < count; i++) {
		GraphType type;
		stream >> type;

		if (GraphType::kGraphText == type) {
			TextGraph* tg = new TextGraph(canvas_);
			tg->DeserializeFrom(stream);
			AddGraph(tg);
		}
		else if (GraphType::kGraphPicture == type) {
			GraphicGraph* tg = new GraphicGraph(canvas_);
			tg->DeserializeFrom(stream);
			AddGraph(tg);
		}
		else if (GraphType::kGraphGroup == type) {
			auto group = new GraphGroup(canvas_);
			group->DeserializeFrom(stream);
			AddGraph(group);
		}
	}
}

void GraphGroup::draw(QPainter& p)
{
	for (auto& g : graphs_)
		g->Draw(p);
}

GraphType GraphGroup::type()
{
	return GraphType::kGraphGroup;
}

void GraphGroup::MoveDown(Graph* graph)
{
	auto ind = graphs_.indexOf(graph);
	if (ind > 0) {
		graphs_.swapItemsAt(ind, ind - 1);
	}
}
void GraphGroup::MoveUp(Graph* graph)
{
	auto ind = graphs_.indexOf(graph);
	if (ind < graphs_.size() - 1) {
		graphs_.swapItemsAt(ind, ind + 1);
	}
}
void GraphGroup::MoveBottom(Graph* graph)
{
	auto ind = graphs_.indexOf(graph);
	if (ind != 0) {
		graphs_.move(ind, 0);
	}
}
void GraphGroup::MoveTop(Graph* graph)
{
	auto ind = graphs_.indexOf(graph);
	if (ind != graphs_.size() - 1) {
		graphs_.move(ind, graphs_.size() - 1);
	}
}