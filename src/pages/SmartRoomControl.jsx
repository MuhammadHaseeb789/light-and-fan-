import React, { useState, useEffect } from 'react';
import { Sun, Moon, Wind, Power, Zap } from 'lucide-react';
import mqtt from 'mqtt';

// Free HiveMQ WebSocket broker URL
const MQTT_BROKER = 'wss://broker.hivemq.com:8884/mqtt';

export default function SmartRoomControl() {
  const [isLightOn, setIsLightOn] = useState(false);
  const [isFanOn, setIsFanOn] = useState(false);
  const [client, setClient] = useState(null);
  const [isConnected, setIsConnected] = useState(false);

  useEffect(() => {
    // Connect to MQTT broker via WebSockets (works securely on Vercel HTTPS!)
    const mqttClient = mqtt.connect(MQTT_BROKER);

    mqttClient.on('connect', () => {
      setIsConnected(true);
      console.log('Connected to MQTT Broker');
    });

    mqttClient.on('error', (err) => {
      console.error('MQTT connection error: ', err);
      setIsConnected(false);
    });

    setClient(mqttClient);

    return () => {
      mqttClient.end();
    };
  }, []);

  const handleLightToggle = () => {
    const newState = !isLightOn;
    setIsLightOn(newState);
    if (client && isConnected) {
      client.publish('myhome/esp32/light', newState ? 'on' : 'off');
    }
  };

  const handleFanToggle = () => {
    const newState = !isFanOn;
    setIsFanOn(newState);
    if (client && isConnected) {
      client.publish('myhome/esp32/fan', newState ? 'on' : 'off');
    }
  };

  return (
    <div className="min-h-screen bg-slate-950 flex items-center justify-center p-6 text-slate-100 font-sans">
      <div className="w-full max-w-md bg-slate-900/80 backdrop-blur-xl border border-slate-800 rounded-3xl p-6 shadow-2xl relative overflow-hidden">
        
        {/* Header */}
        <div className="flex items-center justify-between mb-8 relative z-10">
          <div>
            <h1 className="text-2xl font-bold tracking-tight text-white flex items-center gap-2">
              <Zap className="w-6 h-6 text-indigo-400" />
              Remote IoT Dashboard
            </h1>
            <p className="text-sm text-slate-400 mt-0.5">Controlled via Cloud MQTT</p>
          </div>
          <div className={`flex items-center gap-2 px-3 py-1.5 rounded-full bg-slate-800/80 border border-slate-700/50 text-xs font-medium ${isConnected ? 'text-emerald-400' : 'text-amber-400'}`}>
            <span className={`w-2 h-2 rounded-full ${isConnected ? 'bg-emerald-400 animate-pulse' : 'bg-amber-400'}`} />
            {isConnected ? 'Cloud Online' : 'Connecting...'}
          </div>
        </div>

        {/* Controls Container */}
        <div className="space-y-4 relative z-10">
          
          {/* LIGHT CONTROL */}
          <div className="p-5 rounded-2xl border bg-slate-900/60 border-slate-800 flex items-center justify-between">
            <div className="flex items-center gap-3.5">
              <div className={`p-3 rounded-xl ${isLightOn ? 'bg-amber-500 text-slate-950' : 'bg-slate-800 text-slate-400'}`}>
                {isLightOn ? <Sun className="w-6 h-6" /> : <Moon className="w-6 h-6" />}
              </div>
              <div>
                <h3 className="font-semibold text-white">Smart Light</h3>
                <p className="text-xs text-slate-400">{isLightOn ? 'Active' : 'Turned Off'}</p>
              </div>
            </div>
            <button onClick={handleLightToggle} className={`w-14 h-8 rounded-full transition-colors relative ${isLightOn ? 'bg-amber-500' : 'bg-slate-800'}`}>
              <div className={`absolute top-1 left-1 w-6 h-6 rounded-full bg-white shadow-md transition-transform ${isLightOn ? 'translate-x-6' : ''}`} />
            </button>
          </div>

          {/* FAN CONTROL */}
          <div className="p-5 rounded-2xl border bg-slate-900/60 border-slate-800 flex items-center justify-between">
            <div className="flex items-center gap-3.5">
              <div className={`p-3 rounded-xl ${isFanOn ? 'bg-cyan-500 text-slate-950' : 'bg-slate-800 text-slate-400'}`}>
                <Wind className={`w-6 h-6 ${isFanOn ? 'animate-spin' : ''}`} />
              </div>
              <div>
                <h3 className="font-semibold text-white">Ceiling Fan</h3>
                <p className="text-xs text-slate-400">{isFanOn ? 'Active' : 'Turned Off'}</p>
              </div>
            </div>
            <button onClick={handleFanToggle} className={`w-14 h-8 rounded-full transition-colors relative ${isFanOn ? 'bg-cyan-500' : 'bg-slate-800'}`}>
              <div className={`absolute top-1 left-1 w-6 h-6 rounded-full bg-white shadow-md transition-transform ${isFanOn ? 'translate-x-6' : ''}`} />
            </button>
          </div>

        </div>

      </div>
    </div>
  );
}