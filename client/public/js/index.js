import zoomSdk from '@zoom/appssdk';
import { marked } from 'marked';
import socket from './lib/socket.js';

const onUpdate = ({ transcript, sentiment, faces }) => {


    let titleEl = document.getElementById('title');
    let subtitleEl = document.getElementById('subtitle');

    if (transcript && sentiment) {
        titleEl.innerHTML = transcript;
        subtitleEl.innerHTML = marked.parse(sentiment);
    } else if (faces) {
        titleEl.innerHTML = `There ${Number(faces) === 1 ? 'is' : 'are'} ${faces} ${Number(faces) === 1 ?  'person' : 'people'} on camera`
    }
};

(async () => {
    try {
        const configResponse = await zoomSdk.config({
            capabilities: ['getMeetingUUID'],
        });

        console.debug('Zoom JS SDK Configuration', configResponse);

        const { meetingUUID } = await zoomSdk.getMeetingUUID();

        socket.on('update', onUpdate);
        socket.emit('join', { meetingUUID });
    } catch (e) {
        console.error(e);
    }
})();
