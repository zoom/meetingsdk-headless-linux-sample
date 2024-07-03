import { Server } from 'socket.io';
import debug from 'debug';
import { anthropicKey, appName } from '../config.js';
import consumer from './consumer.js';
import Anthropic from '@anthropic-ai/sdk';

const dbg = debug(`${appName}:socket-io`);

const prompt = `
Perform semantic analysis on each of the transcripts that I send.

Sentiment analysis inspects the given text and identifies the prevailing emotional opinion within the text, 
especially to determine a writer's attitude as positive, negative, or neutral. 

This analysis consists of the following fields. First, score of the sentiment ranges between -1.0 (negative) and 1.0 
(positive) and corresponds to the overall emotional leaning of the text. 

Second, magnitude indicates the overall strength of emotion (both positive and negative) within the given text, 
between 0.0 and +inf. Unlike score, magnitude is not normalized; each expression of emotion within the text 
(both positive and negative) contributes to the text's magnitude (so longer text blocks may have greater magnitudes).
 
You must only respond with the score and magnitude in title case. No explanations or other words should be included. 
If not enough information is returned to determine sentiment you should return 0 values for the the analysis.`;

const claude = new Anthropic({
    apiKey: anthropicKey,
});

function err(socket, code, message) {
    dbg('error', `[${code}] ${message}`);
    socket.emit('error', {
        code,
        message,
    });
}

function checkUUID(socket, uuid) {
    const isUUID = uuid && typeof uuid === 'string';

    if (!isUUID) err(socket, 400, 'Meeting UUID cannot be blank');

    return isUUID;
}

function onConnection(io) {
    return (socket) => {
        let room;

        socket.on('join', ({ meetingUUID }) => {
            if (!checkUUID(socket, meetingUUID)) return;

            dbg('connected');

            if (!room) {
                room = meetingUUID;
                socket.join(room);
            }

            let curText, lastText, result;
            curText = lastText = result = '';

            consumer.socketClient.on('data', (data) => {
                io.to(room).emit('update', { faces: data.toString() });

                //transcriber.sendAudio(data);
            });

            consumer.transcriber.on('transcript', async (transcript) => {
                if (!transcript.text) {
                    return;
                }

                if (curText.length > 32 || lastText.length === 0 && curText.length === 0)
                    io.to(room).emit('update', { transcript: transcript.text });

                const str = ' ' + transcript.text;

                if (transcript.message_type === 'PartialTranscript') {
                    curText += str
                } else {
                    lastText += str


                    let stream = await claude.messages
                        .stream({
                            system: prompt,
                            messages: [
                                { role: 'user', content: transcript.text },
                            ],
                            model: 'claude-3-haiku-20240307',
                            max_tokens: 1024,
                        })
                        .on('text', (sentiment) => {
                            result += sentiment;
                            io.to(room).emit('update', { sentiment: result });
                        });

                    const message = await stream.finalMessage();
                    if (message) {
                        result = '';
                        io.to(room).emit('update', { sentiment: message.text });
                    }
                }
            });
        });
    };
}

/**
 * Initialize the socket.io websocket handler
 * @param {Server} server HTTP Server
 */
export async function startWS(server) {
    const io = new Server(server, {
        transports: ['websocket'],
        maxHttpBufferSize: 1e8,
        pingTimeout: 60000,
    });

    // start socket server and transcription processs
    await consumer.run();

    io.on('connection', onConnection(io));
}
