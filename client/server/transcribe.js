import { AssemblyAI } from 'assemblyai';
import { appName, assemblyAiKey } from '../config.js';
import debug from 'debug';

const dbg = debug(`${appName}:transcriber`);

const client = new AssemblyAI({
    apiKey: assemblyAiKey,
    wordBoost: ['zoom', 'meeting', 'sdk', 'transcription'],
});

export const transcriber = client.realtime.transcriber({
    sampleRate: 32_000,
});

transcriber.on('open', ({ sessionId }) => {
    dbg(`Session opened with ID: ${sessionId}`);
});

transcriber.on('error', (error) => {
    dbg('Error:', error);
});

transcriber.on('close', (code, reason) => dbg('Session closed:', code, reason));

export default {
    transcriber,
};
