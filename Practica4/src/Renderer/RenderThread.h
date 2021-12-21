#pragma once
//no vale herencia de comandos porque hacer muchos news y deletes es malo, y para que funcione la herencia dinamica la cola 
//tendria que ser de punteros que implica hacer new desde la hebra de logica y cuando se lee un comando hacer un delete desde la hebra de render
//union se usa para programacion de sistemas. Nos sirve porque solo vamos a usar una de todos los tipos, entonces queremos que 
//ocupe en memoria como la estructura mas grande, no la suma de todos ellos.
//lo que esté dentro de una union, no puede tener constructor

#include "RenderCommand.h"
#include <atomic>
#include <thread>
#include <condition_variable>
#include <mutex>

template <typename T>
class ConcurrentQueue;

class RenderThread {
public:
	static void Start();
	static void Stop();
	static void AddCommand(const RenderCommand& command);
	static void IncreaseFrames();

	static unsigned short GetFrames();
	static unsigned short GetMaxQueuedFrames();

	static void WaitLock();
	static void SignalLock();

private:
	// Hilo de render
	static std::thread _renderThread;
	
	// Flag para salir del bucle de render
	static std::atomic<bool> _exit;

	// Nº de frames encolados
	static std::atomic<unsigned short> _frames;
	
	// Cola concurrente Lock Free
	static ConcurrentQueue<RenderCommand> _q;

	// Variables para la comunicacion y sincronizacion de los threads de render y logica
	static std::condition_variable _cv;
	static std::mutex _mutex;
	static std::unique_lock<std::mutex> _lock;
	
	// Maximo nº de frames encolados
	static const unsigned short maxQueue;

	// Bucle de procesamiento de comandos de render
	static void run();
};