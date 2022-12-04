#include "MetaSaver.h"
#include "db.h"
#include "Meta.h"
#include <QDateTime>

MetaSaver::MetaSaver(DB* db)
	: db_{ db }
{
	thread_ = new std::thread(&MetaSaver::Run, this);
}

void MetaSaver::Push(Event* event)
{
	bq_.Push(std::move(event));
}

void MetaSaver::Run()
{
	while (true)
	{
		auto event = bq_.Pop();
		if (!event)
			break;

		auto prop = event->meta->PropertyToStream();
		auto snap = event->snapshot.toLocal8Bit().toBase64();

		std::unique_ptr<char> data(new char[event->contents.length() + prop.length() + snap.length() + 256] { 0 });
		int r = sprintf(data.get(), R"(update Memo set DATA="%s", PROP="%s", SNAPSHOT="%s", TIME="%s" where ID=%u;)",
			event->contents.data(),
			prop.data(),
			snap.data(),
			event->time.toLocal8Bit().data(),
			event->meta->Id());

		db_->Query(data.get());
		delete event;
	}
}